//
// Created by Victor Alicino on 19/10/22.
//

#include "AudioPipeline.hpp"

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

AudioPipeline::AudioPipeline(audio_pipeline_cfg_t pipeline_cfg) {
    ESP_LOGD(PIPELINE_TAG, "Opus -> Creating Audio Pipeline");
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    if(this->pipeline != (void*) 0){
        ESP_LOGD(PIPELINE_TAG, "Opus -> Audio Pipeline created");
        this->pipeline_state = AP_STATE_STOPPED;
    } else {
        ESP_LOGE(PIPELINE_TAG, "Opus -> Failed to create Audio Pipeline");
    }
    ESP_LOGD(PIPELINE_TAG, "Opus -> Pipeline successfully  created");
}

void AudioPipeline::register_element(audio_element_handle_t element, const char* name) {
    ESP_LOGD(PIPELINE_TAG, "Opus -> Registering element to pipeline");
    audio_pipeline_register(this->pipeline, element, name);
    ESP_LOGD(PIPELINE_TAG, "Opus -> Element %s registered to pipeline", name);
}

esp_err_t AudioPipeline::link_elements(const char** elements_order, int num_elements) {
    ESP_LOGD(PIPELINE_TAG, "Opus -> Linking elements to pipeline");
    esp_err_t err = ESP_OK;
    err = audio_pipeline_link(this->pipeline, elements_order, num_elements);
    if(num_elements > 0){
        ESP_LOGD(PIPELINE_TAG, "%s -> ", elements_order[0]);
        for(int i=1; i<=num_elements; i++){
            if (i == num_elements){
                ESP_LOGD(PIPELINE_TAG, "%s", elements_order[i]);
            } else {
                ESP_LOGD(PIPELINE_TAG, "%s -> ", elements_order[i]);
            }
        }
    }
    ESP_LOGD(PIPELINE_TAG, "Opus -> Elements linked to pipeline");
    return err;
}
