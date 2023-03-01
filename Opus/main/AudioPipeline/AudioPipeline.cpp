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

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

static const char *PIPELINE_TAG = __FILENAME__;

AudioPipeline::AudioPipeline(audio_pipeline_cfg_t pipeline_cfg, const char* name) {
    ESP_LOGD(PIPELINE_TAG, "Creating Audio Pipeline");
    this->name = name;
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    if(this->pipeline != (void*) 0){
        this->pipeline_state = AP_STATE_STOPPED;
    } else {
        ESP_LOGE(this->name, "Failed to create Audio Pipeline");
    }
    ESP_LOGD(this->name, "Pipeline successfully created");
}

audio_pipeline_handle_t AudioPipeline::get_pipeline() {
    return this->pipeline;
}

void AudioPipeline::register_element(audio_element_handle_t element, const char* name) {
    esp_err_t err = audio_pipeline_register(this->pipeline, element, name);
    if (err == ESP_OK) ESP_LOGD(this->name, "Element ==> %s <== registered to pipeline", name);
    else ESP_LOGE(this->name, "Failed to register element ==> %s <== to pipeline", name);
}

esp_err_t AudioPipeline::link_elements(const char** elements_order, int num_elements) {
    ESP_LOGD(this->name, "Linking elements to pipeline");
    esp_err_t err = ESP_OK;
    err = audio_pipeline_link(this->pipeline, elements_order, num_elements);
    if(num_elements > 0){
        printf("%s = {%s -> ", this->name, elements_order[0]);
        for(int i=1; i<num_elements; i++){
            if (i == (num_elements - 1)){
                printf("%s}\n", elements_order[i]);
            } else {
                printf("%s -> ", elements_order[i]);
            }
        }
    }
    ESP_LOGD(this->name, "Elements linked to pipeline");
    return err;
}

void AudioPipeline::run() {
    ESP_LOGD(this->name, "Opus -> Running pipeline");
    audio_pipeline_run(this->pipeline);
    this->pipeline_state = AP_STATE_RUNNING;
    ESP_LOGD(this->name, "Opus -> Pipeline running");
}
