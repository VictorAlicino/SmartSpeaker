//
// Created by Victor Alicino on 23/05/22.
//

#include "WebRadio.hpp"

#include "esp_log.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "i2s_stream.h"
#include "audio_pipeline.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"
#include "MQTT.hpp"

#include "math.h"

const char* WEBRADIO_TAG = __FILENAME__;

WebRadio::WebRadio() {
    ESP_LOGD(__FILENAME__, "Construindo %s", __FILENAME__);
    this->volume_old = 50;
}

esp_err_t WebRadio::add_uri(std::string url) {
    //TODO Adicionar funcionalidade para múltiplas URI
    this->urls.push_back(url);
    this->url_available = true;
    if(esp_log_level_get(WEBRADIO_TAG) == ESP_LOG_INFO){
        printf("\n");
    }
    ESP_LOGI(WEBRADIO_TAG, "Added URI: %s\n"
                           "                      Total URIs registered:%d\n", url.c_str(), this->urls.size());

    if(esp_log_level_get(WEBRADIO_TAG) == ESP_LOG_DEBUG){
        printf("                      The follow URL are available:\n");
        for(int i=0; i < this->urls.size(); i++) {
            printf("                      %s\n", urls.at(i).c_str());
        }
        printf("\n");
    }
    audio_element_set_uri(this->http_stream_reader, urls.at(0).c_str());
    return ESP_OK;
}

esp_err_t WebRadio::link_to_pipeline(AUDIO_STREAM reader, AUDIO_CODEC codec, AUDIO_STREAM writer) {
    esp_err_t er = ESP_OK;

    // Setting up Reader
    if(reader == HTTP_STREAM) {
        audio_pipeline_register(this->pipeline, this->http_stream_reader, "http");
        this->link_tag.push_back("http");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "http");
    } else er = ESP_FAIL;

    // Setting up CODEC
    if(codec == OGG_DECODER) {
        audio_pipeline_register(this->pipeline, this->ogg_decoder, "ogg");
        this->link_tag.push_back("ogg");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "ogg");
    } else er = ESP_FAIL;

    // Setting up Writer
    if(writer == I2S_STREAM) {
        audio_pipeline_register(this->pipeline, this->i2s_stream_writer, "i2s");
        this->link_tag.push_back("i2s");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "i2s");
    } else er = ESP_FAIL;

    // Linking all to PIPELINE
    const char* r = link_tag.at(0).c_str();
    const char* c = link_tag.at(1).c_str();
    const char* w = link_tag.at(2).c_str();
    const char* link_tag_t[3] = {r, c, w};
    er = audio_pipeline_link(this->pipeline, &link_tag_t[0], link_tag.size());

    if (er == ESP_OK) ESP_LOGD(WEBRADIO_TAG, "Elements linked to pipeline");

    return er;
}

esp_err_t WebRadio::change_volume_to(int volume){
    esp_err_t err = ESP_OK;
    int cur_volume;
    const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
    audio_hal_get_volume(this->board_handle->audio_hal, &cur_volume);
    ESP_LOGI(WEBRADIO_TAG, "Changing Volume from %d to %d", cur_volume, volume);

    if(cur_volume < volume){
        for(;cur_volume < volume; cur_volume++) {
            audio_hal_set_volume(this->board_handle->audio_hal, cur_volume);
            vTaskDelay(xDelay*100);
        }
    }else{
        for(;cur_volume > volume; cur_volume--) {
            audio_hal_set_volume(this->board_handle->audio_hal, cur_volume);
            vTaskDelay(xDelay*300);
        }
    }

    return err;
}

esp_err_t WebRadio::pause() {
    audio_hal_get_volume(this->board_handle->audio_hal, &this->volume_old);
    change_volume_to(0);
    ESP_LOGI(WEBRADIO_TAG, "Sound Paused");
    return ESP_OK;
}

esp_err_t WebRadio::play() {
    change_volume_to(this->volume_old);
    ESP_LOGI(WEBRADIO_TAG, "Sound Play");
    return ESP_OK;
}

esp_err_t WebRadio::loop() {
    int player_volume;
    audio_hal_get_volume(this->board_handle->audio_hal, &player_volume);
    //audio_hal_get_volume(this->board_handle->audio_hal, &volume);

    ESP_LOGI(WEBRADIO_TAG, "[ 3 ] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), this->evt);

    ESP_LOGI(WEBRADIO_TAG, "[3.1] Initialize keys on board");
    audio_board_key_init(set);

    while(this->activated){
        audio_element_state_t el_state = audio_element_get_state(this->i2s_stream_writer);
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(this->evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(WEBRADIO_TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
            && msg.source == (void *) this->ogg_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {
                    0
            };
            audio_element_getinfo(this->ogg_decoder, &music_info);

            ESP_LOGI(WEBRADIO_TAG, "[ * ] Receive music info from ogg decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(this->i2s_stream_writer, &music_info);
            i2s_stream_set_clk(this->i2s_stream_writer, music_info.sample_rates,
                               music_info.bits, music_info.channels);
            continue;
        }

        // Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) this->i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(WEBRADIO_TAG, "[ * ] Stop event received");
            break;
        }

        if ((msg.source_type == PERIPH_ID_TOUCH || msg.source_type == PERIPH_ID_BUTTON || msg.source_type == PERIPH_ID_ADC_BTN)
            && (msg.cmd == PERIPH_TOUCH_TAP || msg.cmd == PERIPH_BUTTON_PRESSED || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) {
            if ((int) msg.data == get_input_play_id()) {
                ESP_LOGI(WEBRADIO_TAG, "[ * ] [Play] touch tap event");
                switch (el_state) {
                    case AEL_STATE_INIT :
                        ESP_LOGI(WEBRADIO_TAG, "[ * ] Starting audio pipeline");
                        audio_pipeline_run(pipeline);
                        break;
                    case AEL_STATE_RUNNING :
                        ESP_LOGI(WEBRADIO_TAG, "[ * ] Pausing audio pipeline");
                        audio_pipeline_pause(pipeline);
                        break;
                    case AEL_STATE_PAUSED :
                        ESP_LOGI(WEBRADIO_TAG, "[ * ] Resuming audio pipeline");
                        audio_pipeline_resume(pipeline);
                        break;
                    case AEL_STATE_FINISHED :
                        ESP_LOGI(WEBRADIO_TAG, "[ * ] Rewinding audio pipeline");
                        audio_pipeline_reset_ringbuffer(pipeline);
                        audio_pipeline_reset_elements(pipeline);
                        audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
                        audio_pipeline_run(pipeline);
                        break;
                    default :
                        ESP_LOGI(WEBRADIO_TAG, "[ * ] Not supported state %d", el_state);
                }
            } else if ((int) msg.data == get_input_set_id()) {
                ESP_LOGI(WEBRADIO_TAG, "[ * ] [Set] touch tap event");
                ESP_LOGI(WEBRADIO_TAG, "[ * ] Stopping audio pipeline");
                break;
            } else if ((int) msg.data == get_input_mode_id()) {
                ESP_LOGI(WEBRADIO_TAG, "[ * ] [mode] tap event");
                audio_pipeline_stop(pipeline);
                audio_pipeline_wait_for_stop(pipeline);
                audio_pipeline_terminate(pipeline);
                audio_pipeline_reset_ringbuffer(pipeline);
                audio_pipeline_reset_elements(pipeline);
                audio_pipeline_run(pipeline);
            } else if ((int) msg.data == get_input_volup_id()) {
                ESP_LOGI(WEBRADIO_TAG, "[ * ] [Vol+] touch tap event");
                player_volume += 10;
                if (player_volume > 100) {
                    player_volume = 100;
                }
                audio_hal_set_volume(this->board_handle->audio_hal, player_volume);
                ESP_LOGI(WEBRADIO_TAG, "[ * ] Volume set to %d %%", player_volume);
            } else if ((int) msg.data == get_input_voldown_id()) {
                ESP_LOGI(WEBRADIO_TAG, "[ * ] [Vol-] touch tap event");
                player_volume -= 10;
                if (player_volume < 0) {
                    player_volume = 0;
                }
                audio_hal_set_volume(this->board_handle->audio_hal, player_volume);
                ESP_LOGI(WEBRADIO_TAG, "[ * ] Volume set to %d %%", player_volume);
            }
        }
        audio_hal_get_volume(this->board_handle->audio_hal, &player_volume);
    }
    return ESP_OK;
}