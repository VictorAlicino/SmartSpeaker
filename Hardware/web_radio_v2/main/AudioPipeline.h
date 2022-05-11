//
// Created by Victor Alicino on 11/05/22.
//

#ifndef WEB_RADIO_V2_AUDIOPIPELINE_H
#define WEB_RADIO_V2_AUDIOPIPELINE_H

#include <vector>
#include <string>
#include "audio_element.h"

class AudioPipeline {
private:
    audio_pipeline_handle_t pipeline;
    std::vector<std::string>urls;
    std::vector<char *>link_tag;

public:
    AudioPipeline();
    esp_err_t register_to_pipeline(audio_element_handle_t audio_element_handle, std::string name);
    esp_err_t link_to_pipeline();
    esp_err_t add_uri(std::string url);
};


#endif //WEB_RADIO_V2_AUDIOPIPELINE_H
