//
// Created by Victor Alicino on 11/05/22.
//

#ifndef WEB_RADIO_V2_AUDIOPIPELINE_H
#define WEB_RADIO_V2_AUDIOPIPELINE_H

#include <vector>
#include <string>
#include "audio_element.h"
#include "audio_event_iface.h"
#include "audio_pipeline.h"
#include "WebConnection.hpp"
#include "AudioStream.hpp"

class AudioPipeline {
private:
    //Pipeline
    bool url_available;
    bool activated;
    audio_pipeline_handle_t pipeline;
    std::vector<std::string>urls;
    std::vector<std::string>link_tag;

    //Event handlers
    audio_event_iface_handle_t evt;

public:
    AudioPipeline();
    esp_err_t register_to_pipeline(audio_element_handle_t audio_element_handle, std::string name);
    esp_err_t link_to_pipeline();
    esp_err_t add_uri(std::string url);
    esp_err_t setup_event(WebConnection network);
    esp_err_t run();
    esp_err_t loop(AudioStream &Stream);
    audio_event_iface_handle_t get_evt();
    esp_err_t stop();
};


#endif //WEB_RADIO_V2_AUDIOPIPELINE_H
