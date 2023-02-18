//
// Created by victo on 18/01/2023.
//

#include "A2DP_HF.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_client_api.h"
#include "bluetooth_service.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "raw_stream.h"
#include "connections_macros.h"

#include <string>
#include <cstring>

#define HFP_RESAMPLE_RATE 16000

static bool is_get_hfp = true;

const char* A2DP_HF_TAG = __FILENAME__;


A2DP_HF* A2DP_HF::instance = nullptr;

A2DP_HF* A2DP_HF::get_instance() {
    if (instance == nullptr) {
        instance = new A2DP_HF();
    }
    return instance;
}

A2DP_HF::A2DP_HF() {}

void A2DP_HF::config(std::string device_name, bluetooth_service_mode_t mode) {
    bluetooth_service_cfg_t bt_cfg = {
            .device_name = device_name.c_str(),
            .remote_name = device_name.c_str(),
            .mode = mode,
    };
    this->bt_cfg = bt_cfg;
}


void A2DP_HF::init(){
    bluetooth_service_start(&this->bt_cfg);
    esp_hf_client_register_callback(bluetooth_service_hf_client_cb);
    esp_hf_client_init();
    this->bt_stream_reader = nullptr;
    this->raw_read = nullptr;
}

esp_err_t A2DP_HF::bt_stream_reader_init(){
    if (this->bt_stream_reader == nullptr) {
        this->bt_stream_reader = bluetooth_service_create_stream();
        if (this->bt_stream_reader == NULL) {
            ESP_LOGE(A2DP_HF_TAG, "Bluetooth stream reader create failed");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t A2DP_HF::raw_read_init(){
    if (this->raw_read == nullptr) {
        raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
        raw_cfg.type = AUDIO_STREAM_READER;
        this->raw_read = raw_stream_init(&raw_cfg);
        if (this->raw_read == NULL) {
            ESP_LOGE(A2DP_HF_TAG, "Raw stream reader create failed");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

audio_element_handle_t A2DP_HF::get_bt_stream_reader(){
    return this->bt_stream_reader;
}

audio_element_handle_t A2DP_HF::get_raw_read(){
    return this->raw_read;
}

static void bt_app_hf_client_audio_open(void)
{
    A2DP_HF* a2dp_hf = A2DP_HF::get_instance();
    audio_element_handle_t bt_stream_reader = a2dp_hf->get_bt_stream_reader();

    ESP_LOGE(A2DP_HF_TAG, "bt_app_hf_client_audio_open");
    int sample_rate = HFP_RESAMPLE_RATE;
    audio_element_info_t bt_info = {0};
    audio_element_getinfo(bt_stream_reader, &bt_info);
    bt_info.sample_rates = sample_rate;
    bt_info.channels = 1;
    bt_info.bits = 16;
    audio_element_setinfo(bt_stream_reader, &bt_info);
    audio_element_report_info(bt_stream_reader);
}

static void bt_app_hf_client_audio_close(void)
{
    A2DP_HF* a2dp_hf = A2DP_HF::get_instance();
    audio_element_handle_t bt_stream_reader = a2dp_hf->get_bt_stream_reader();

    ESP_LOGE(A2DP_HF_TAG, "bt_app_hf_client_audio_close");
    int sample_rate = periph_bluetooth_get_a2dp_sample_rate();
    audio_element_info_t bt_info = {0};
    audio_element_getinfo(bt_stream_reader, &bt_info);
    bt_info.sample_rates = sample_rate;
    bt_info.channels = 2;
    bt_info.bits = 16;
    audio_element_setinfo(bt_stream_reader, &bt_info);
    audio_element_report_info(bt_stream_reader);
}

static uint32_t bt_app_hf_client_outgoing_cb(uint8_t *p_buf, uint32_t sz)
{
    A2DP_HF* a2dp_hf = A2DP_HF::get_instance();
    audio_element_handle_t raw_read = a2dp_hf->get_raw_read();

    int out_len_bytes = 0;
    char *enc_buffer = (char *)audio_malloc(sz);
    AUDIO_MEM_CHECK(A2DP_HF_TAG, enc_buffer, return 0);
    if (is_get_hfp) {
        out_len_bytes = raw_stream_read(raw_read, enc_buffer, sz);
    }

    if (out_len_bytes == sz) {
        is_get_hfp = false;
        memcpy(p_buf, enc_buffer, out_len_bytes);
        free(enc_buffer);
        return sz;
    } else {
        is_get_hfp = true;
        free(enc_buffer);
        return 0;
    }
}

static void bt_app_hf_client_incoming_cb(const uint8_t *buf, uint32_t sz)
{
    A2DP_HF* a2dp_hf = A2DP_HF::get_instance();
    audio_element_handle_t bt_stream_reader = a2dp_hf->get_bt_stream_reader();

    if (bt_stream_reader) {
        if (audio_element_get_state(bt_stream_reader) == AEL_STATE_RUNNING) {
            audio_element_output(bt_stream_reader, (char *)buf, sz);
            esp_hf_client_outgoing_data_ready();
        }
    }
}

void bluetooth_service_hf_client_cb(esp_hf_client_cb_event_t event, esp_hf_client_cb_param_t *param) {/* callback for HF_CLIENT */
    if (event <= ESP_HF_CLIENT_RING_IND_EVT) {
        ESP_LOGE(A2DP_HF_TAG, "APP HFP event: %s", c_hf_evt_str[event]);
    } else {
        ESP_LOGE(A2DP_HF_TAG, "APP HFP invalid event %d", event);
    }

    switch (event) {
        case ESP_HF_CLIENT_CONNECTION_STATE_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--connection state %s, peer feats 0x%x, chld_feats 0x%x",
                     c_connection_state_str[param->conn_stat.state],
                     param->conn_stat.peer_feat,
                     param->conn_stat.chld_feat);
            break;
        case ESP_HF_CLIENT_AUDIO_STATE_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--audio state %s",
                     c_audio_state_str[param->audio_stat.state]);
#if CONFIG_HFP_AUDIO_DATA_PATH_HCI
            if ((param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_CONNECTED)
            || (param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_CONNECTED_MSBC)) {
            bt_app_hf_client_audio_open();
            esp_hf_client_register_data_callback(bt_app_hf_client_incoming_cb,
                                                 bt_app_hf_client_outgoing_cb);
        } else if (param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_DISCONNECTED) {
            bt_app_hf_client_audio_close();
        }
#endif /* #if CONFIG_HFP_AUDIO_DATA_PATH_HCI */
            break;
        case ESP_HF_CLIENT_BVRA_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--VR state %s",
                     c_vr_state_str[param->bvra.value]);
            break;
        case ESP_HF_CLIENT_CIND_SERVICE_AVAILABILITY_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--NETWORK STATE %s",
                     c_service_availability_status_str[param->service_availability.status]);
            break;
        case ESP_HF_CLIENT_CIND_ROAMING_STATUS_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--ROAMING: %s",
                     c_roaming_status_str[param->roaming.status]);
            break;
        case ESP_HF_CLIENT_CIND_SIGNAL_STRENGTH_EVT:
            ESP_LOGE(A2DP_HF_TAG, "-- signal strength: %d",
                     param->signal_strength.value);
            break;
        case ESP_HF_CLIENT_CIND_BATTERY_LEVEL_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--battery level %d",
                     param->battery_level.value);
            break;
        case ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--operator name: %s",
                     param->cops.name);
            break;
        case ESP_HF_CLIENT_CIND_CALL_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--Call indicator %s",
                     c_call_str[param->call.status]);
            break;
        case ESP_HF_CLIENT_CIND_CALL_SETUP_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--Call setup indicator %s",
                     c_call_setup_str[param->call_setup.status]);
            break;
        case ESP_HF_CLIENT_CIND_CALL_HELD_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--Call held indicator %s",
                     c_call_held_str[param->call_held.status]);
            break;
        case ESP_HF_CLIENT_BTRH_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--response and hold %s",
                     c_resp_and_hold_str[param->btrh.status]);
            break;
        case ESP_HF_CLIENT_CLIP_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--clip number %s",
                     (param->clip.number == NULL) ? "NULL" : (param->clip.number));
            break;
        case ESP_HF_CLIENT_CCWA_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--call_waiting %s",
                     (param->ccwa.number == NULL) ? "NULL" : (param->ccwa.number));
            break;
        case ESP_HF_CLIENT_CLCC_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--Current call: idx %d, dir %s, state %s, mpty %s, number %s",
                     param->clcc.idx,
                     c_call_dir_str[param->clcc.dir],
                     c_call_state_str[param->clcc.status],
                     c_call_mpty_type_str[param->clcc.mpty],
                     (param->clcc.number == NULL) ? "NULL" : (param->clcc.number));
            break;
        case ESP_HF_CLIENT_VOLUME_CONTROL_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--volume_target: %s, volume %d",
                     c_volume_control_target_str[param->volume_control.type],
                     param->volume_control.volume);
            break;
        case ESP_HF_CLIENT_AT_RESPONSE_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--AT response event, code %d, cme %d",
                     param->at_response.code, param->at_response.cme);
            break;
        case ESP_HF_CLIENT_CNUM_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--subscriber type %s, number %s",
                     c_subscriber_service_type_str[param->cnum.type],
                     (param->cnum.number == NULL) ? "NULL" : param->cnum.number);
            break;
        case ESP_HF_CLIENT_BSIR_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--inband ring state %s",
                     c_inband_ring_state_str[param->bsir.state]);
            break;
        case ESP_HF_CLIENT_BINP_EVT:
            ESP_LOGE(A2DP_HF_TAG, "--last voice tag number: %s",
                     (param->binp.number == NULL) ? "NULL" : param->binp.number);
            break;
        default:
            ESP_LOGE(A2DP_HF_TAG, "HF_CLIENT EVT: %d", event);
            break;
    }
}

