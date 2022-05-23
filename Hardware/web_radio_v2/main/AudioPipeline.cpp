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
    ESP_LOGD(__FILENAME__, "Creating Audio Pipeline                               [AudioPipeline: 1/1]");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(this->pipeline);
    this->url_available = false;
    this->activated = false;
}

esp_err_t AudioPipeline::register_to_pipeline(audio_element_handle_t audio_element_handle, std::string tag) {
    // Cloned from audio_pipeline.c and adapted to C++
    esp_err_t er = ESP_OK;
    audio_pipeline_register(this->pipeline, audio_element_handle, tag.c_str());
    this->link_tag.push_back(tag);
    ESP_LOGD(__FILENAME__, "Element <<%s>> registered to Pipeline", tag.c_str());
    return er;
}

esp_err_t AudioPipeline::link_to_pipeline() {
    esp_err_t out;
    const char* a = link_tag.at(0).c_str();
    const char* b = link_tag.at(1).c_str();
    const char* c = link_tag.at(2).c_str();
    const char* link_tag_t[3] = {a, b, c};
    out = audio_pipeline_link(this->pipeline, &link_tag_t[0], link_tag.size());
    if (out == ESP_OK) {
        ESP_LOGD(__FILENAME__, "Elements linked to pipeline");
        return out;
    }
    throw ESP_FAIL;
}

esp_err_t AudioPipeline::add_uri(std::string url, AudioStream &Stream) {
    this->urls.push_back(url);
    this->url_available = true;
    ESP_LOGI(__FILENAME__, "\nAdded URI:\n%s\nTotal URIs registered:%d", url.c_str(), this->urls.size());
    ESP_LOGD(__FILENAME__, "The follow URL are available:");
    if(esp_log_level_get(__FILENAME__) == ESP_LOG_DEBUG){
        for(int i=0; i < urls.size(); i++) {
            printf("%s\n", urls.at(i).c_str());
        }
    }
    audio_element_set_uri(Stream.get_http_stream_reader(), urls.at(0).c_str());
    return ESP_OK;
}

esp_err_t AudioPipeline::setup_event(WebConnection network) {
    ESP_LOGD(__FILENAME__, "Setup event listener                                   [Setup Events: 1/4]");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    this->evt = audio_event_iface_init(&evt_cfg);
    ESP_LOGI(__FILENAME__, "Event listener created                         [X]");


    ESP_LOGD(__FILENAME__, "Listening event from all elements of pipeline          [Setup Events: 2/4]");
    audio_pipeline_set_listener(this->pipeline, this->evt);
    ESP_LOGI(__FILENAME__, "Listening event from all elements of pipeline  [X]");


    ESP_LOGD(__FILENAME__, "Listening event from peripherals                       [Setup Events: 3/4]");
    audio_event_iface_set_listener(
            esp_periph_set_get_event_iface(network.get_set()),
            this->evt);
    ESP_LOGI(__FILENAME__, "Listening event from peripherals               [X]");


    return ESP_OK;
}

esp_err_t AudioPipeline::run() {
    audio_pipeline_run(this->pipeline);
    this->activated = true;
    return ESP_OK;
}

esp_err_t AudioPipeline::loop(AudioStream &Stream) {
    while(this->activated){
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(this->evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(__FILENAME__, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
            && msg.source == (void *) Stream.get_ogg_decoder()
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
                audio_element_info_t music_info = {
                        0
                };
                audio_element_getinfo(Stream.get_ogg_decoder(), &music_info);

                ESP_LOGI(__FILENAME__, "[ * ] Receive music info from ogg decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

                audio_element_setinfo(Stream.get_i2s_stream_writer(), &music_info);
                i2s_stream_set_clk(Stream.get_i2s_stream_writer(), music_info.sample_rates,
                                   music_info.bits, music_info.channels);
                continue;
        }

        // Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) Stream.get_i2s_stream_writer()
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(__FILENAME__, "[ * ] Stop event received");
            break;
        }

    }
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