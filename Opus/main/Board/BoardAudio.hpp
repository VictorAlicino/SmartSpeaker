//
// Created by victo on 27/01/2023.
//

#ifndef SMARTSPEAKER_BOARDAUDIO_HPP
#define SMARTSPEAKER_BOARDAUDIO_HPP

#include "board.h"
#include "Device.hpp"
#include "AudioPipeline/AudioPipeline.hpp"

class BoardAudio {
private:
    //Singleton instance
    static BoardAudio* instance;

    audio_board_handle_t board_handle; // Audio board handle

    audio_event_iface_handle_t evt_handle; // Audio event interface handle

    BoardAudio();

public:

    /**
     * @brief Get singleton instance
     * @return Singleton instance
     */
    static BoardAudio* get_instance();

    esp_err_t audio_hal_init();

    esp_err_t audio_event_interface_init(
            audio_event_iface_cfg_t* cfg,
            AudioPipeline* pipeline);

    void set_volume(int volume);

    void set_volume_with_fade(int desired_volume, int fade_time_ms);

    void mute();

    int get_volume();

    audio_board_handle_t get_board_handle();

    audio_event_iface_handle_t get_evt_handle();
};


#endif //SMARTSPEAKER_BOARDAUDIO_HPP
