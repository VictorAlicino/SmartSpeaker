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
    int volume_old;

    void on_music_info();
    void on_play_button_pressed();

public:
    WebRadio();
    esp_err_t link_to_pipeline(AUDIO_STREAM reader, AUDIO_CODEC codec, AUDIO_STREAM writer);
    esp_err_t add_uri(std::string url);
    esp_err_t loop();
    esp_err_t change_volume_to(int volume);

};


#endif //WEB_RADIO_V2_WEBRADIO_H