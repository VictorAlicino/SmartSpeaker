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

//Singleton get instance
AudioPipeline &AudioPipeline::getInstance() {
    static AudioPipeline instance;
    return instance;
};

AudioPipeline::AudioPipeline() {
    ESP_LOGD(__FILENAME__, "Opus -> Creating Audio Pipeline");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    if(this->pipeline != (void*) 0){
        ESP_LOGD(__FILENAME__, "Opus -> Audio Pipeline created");
        this->pipeline_state = AP_STATE_STOPPED;
    } else {
        ESP_LOGE(__FILENAME__, "Opus -> Failed to create Audio Pipeline");
    }
}
