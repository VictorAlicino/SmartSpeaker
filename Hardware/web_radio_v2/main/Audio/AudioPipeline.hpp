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

enum PIPELINE_STATE{
    ACTIVE,
    PAUSED,
    STOPPED
};

class AudioPipeline {
protected:
    //Pipeline
    bool url_available;
    int pipeline_state;
    audio_pipeline_handle_t pipeline;
    std::vector<std::string>link_tag;

    //Event handlers
    audio_event_iface_handle_t evt;

public:
    // Pipeline configurations
    AudioPipeline();
    esp_err_t setup_event(WebConnection network);

    // Pipeline controls
    esp_err_t run();
    esp_err_t pause();
    esp_err_t resume();
    esp_err_t restart();
    esp_err_t stop();

    //Pipeline getters and setters
    audio_event_iface_handle_t get_evt();
};


#endif //WEB_RADIO_V2_AUDIOPIPELINE_H
