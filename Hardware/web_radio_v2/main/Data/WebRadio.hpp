//
// Created by Victor Alicino on 23/05/22.
//

#ifndef WEB_RADIO_V2_WEBRADIO_H
#define WEB_RADIO_V2_WEBRADIO_H

#include "AudioStream.hpp"
#include "AudioPipeline.hpp"
#include "WebConnection.hpp"
#include <string>


class WebRadio : public AudioStream, public AudioPipeline{
private:
    std::vector<std::string>urls;

public:
    WebRadio(): urls() {}
    esp_err_t add_uri(std::string url);
    esp_err_t loop();

};


#endif //WEB_RADIO_V2_WEBRADIO_H