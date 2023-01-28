//
// Created by Victor Alicino on 19/10/22.
//

#include "AudioPipeline.h"

#include "esp_log.h"
#include "audio_mem.h"
#include "i2s_stream.h"
#include "audio_error.h"
#include "audio_common.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "esp_peripherals.h"
#include "audio_event_iface.h"

#include "Board/Device.hpp"

static const char *PIPELINE_TAG = __FILENAME__;

AudioPipeline::AudioPipeline() {
    ESP_LOGD(__FILENAME__, "Opus -> Creating Audio Pipeline");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG(); // ESP-ADF Macro | Default pipeline config
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    if(this->pipeline != (void*) 0){
        ESP_LOGD(PIPELINE_TAG, "Opus -> Audio Pipeline created");
        this->pipeline_state = AP_STATE_STOPPED;
    } else {
        ESP_LOGE(PIPELINE_TAG, "Opus -> Failed to create Audio Pipeline");
    }

    ESP_LOGD(PIPELINE_TAG, "Opus -> Creating Event Interface Listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    this->evt = audio_event_iface_init(&evt_cfg);
    if(this->evt != (void*) 0){
        ESP_LOGD(PIPELINE_TAG, "Opus -> Event Interface Listener created");
    } else {
        ESP_LOGE(PIPELINE_TAG, "Opus -> Failed to create Event Interface Listener");
    }

    esp_err_t err;
    err = audio_pipeline_set_listener(this->pipeline, this->evt);
    if(err == ESP_OK){
        ESP_LOGD(PIPELINE_TAG, "Opus -> Event Listener linked to Pipeline");
    } else {
        ESP_LOGE(PIPELINE_TAG, "Opus -> Failed to link Event Listener to Pipeline");
    }

    /*Device *Board = Device::get_instance(BOARD_TYPE::LYRAT_V4_3);
    err = audio_event_iface_set_listener(
            Board->get_peripheral_handle(),
            this->evt);
    if(err == ESP_OK){
        ESP_LOGD(PIPELINE_TAG, "Opus -> Event Listener linked to Peripherals");
    } else {
        ESP_LOGE(PIPELINE_TAG, "Opus -> Failed to link Event Listener to Peripherals");
        throw std::runtime_error("Failed to link Event Listener to Peripherals");
    }*/
}
