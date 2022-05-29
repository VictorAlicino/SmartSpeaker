//
// Created by Victor Alicino on 11/05/22.
//

#include "AudioPipeline.hpp"


#include <vector>
#include <string>
#include "esp_log.h"
#include "audio_mem.h"
#include "i2s_stream.h"
#include "audio_error.h"
#include "audio_common.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "esp_peripherals.h"
#include "audio_event_iface.h"

AudioPipeline::AudioPipeline() {
    ESP_LOGD(__FILENAME__, "Creating Audio Pipeline");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(this->pipeline);
    this->url_available = false;
    this->activated = false;
}

esp_err_t AudioPipeline::setup_event(WebConnection network) {
    ESP_LOGD(__FILENAME__, "Setup event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    this->evt = audio_event_iface_init(&evt_cfg);
    ESP_LOGI(__FILENAME__, "Event listener created");


    ESP_LOGD(__FILENAME__, "Listening event from all elements of pipeline");
    audio_pipeline_set_listener(this->pipeline, this->evt);
    ESP_LOGI(__FILENAME__, "Listening event from all elements of pipeline");


    ESP_LOGD(__FILENAME__, "Listening event from peripherals");
    audio_event_iface_set_listener(
            esp_periph_set_get_event_iface(network.get_set()),
            this->evt);
    ESP_LOGI(__FILENAME__, "Listening event from peripherals");


    return ESP_OK;
}

esp_err_t AudioPipeline::run() {
    audio_pipeline_run(this->pipeline);
    this->activated = true;
    return ESP_OK;
}

esp_err_t AudioPipeline::restart() {
    audio_pipeline_stop(this->pipeline);
    audio_pipeline_wait_for_stop(this->pipeline);
    audio_pipeline_terminate(this->pipeline);
    audio_pipeline_reset_ringbuffer(this->pipeline);
    audio_pipeline_reset_elements(this->pipeline);
    audio_pipeline_run(this->pipeline);
    return ESP_OK;
}

audio_event_iface_handle_t AudioPipeline::get_evt() {
    return this->evt;
}

esp_err_t AudioPipeline::stop() {
    audio_pipeline_stop(this->pipeline);
    audio_pipeline_wait_for_stop(this->pipeline);
    audio_pipeline_terminate(this->pipeline);

    //TODO: TERMINAR A FINALIZAÇÃO DA EXECUÇÃO
    /*
    for(int i=0; i>this->link_tag.size(); i++){
        audio_pipeline_unregister(this->pipeline, )
    }
     */
    return ESP_OK;
}