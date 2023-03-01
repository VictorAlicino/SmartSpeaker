//
// Created by victo on 27/01/2023.
//

#include "BoardAudio.hpp"

#include "audio_hal.h"
#include "audio_event_iface.h"
#include "board.h"

BoardAudio* BoardAudio::instance = nullptr;

BoardAudio* BoardAudio::get_instance() {
    if (instance == nullptr) {
        instance = new BoardAudio();
    }
    return instance;
}

BoardAudio::BoardAudio() {
    this->board_handle = nullptr;
}

esp_err_t BoardAudio::init() {
    esp_err_t err = ESP_OK;
    this->board_handle = audio_board_init();
    err = audio_hal_ctrl_codec(this->board_handle->audio_hal,
                               AUDIO_HAL_CODEC_MODE_DECODE,
                               AUDIO_HAL_CTRL_START);
    return err;
}