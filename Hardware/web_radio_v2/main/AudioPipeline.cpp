//
// Created by Victor Alicino on 11/05/22.
//

#include "AudioPipeline.h"


#include "audio_pipeline.h"

AudioPipeline::AudioPipeline() {
    ESP_LOGD(__FILENAME__, "Creating Audio Pipeline                               [AudioPipeline: 1/1]");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(this->pipeline);
}

esp_err_t AudioPipeline::register_to_pipeline(audio_element_handle_t audio_element_handle, std::string name) {
    esp_err_t out;
    out = audio_pipeline_register(this->pipeline, audio_element_handle, name.c_str());
    if (out == ESP_OK) {
        this->link_tag.push_back(name.c_str());
        ESP_LOGD(__FILENAME__, "Element <<%s>> registered to Pipeline", name.c_str());
        return out;
    }
    throw ESP_FAIL;
}

esp_err_t AudioPipeline::link_to_pipeline() {
    esp_err_t out;
    out = audio_pipeline_link(this->pipeline, this->link_tag.data(), this->link_tag.size());
    if (out == ESP_OK) {
        ESP_LOGD(__FILENAME__, "Elements linked to pipeline");
        return out;
    }
    throw ESP_FAIL;
}

esp_err_t AudioPipeline::add_uri(std::string url) {
    this->urls.push_back(url);
    ESP_LOGI(__FILENAME__, "\nAdded URI:\n%s\nTotal URIs registered:%d", url.c_str(), this->urls.size());
    for(int i=0; i < urls.size(); i++) {
        
    }
    return ESP_OK;
}
