//
// Created by victo on 27/01/2023.
//

#include "AudioHAL.hpp"

#include "audio_hal.h"
#include "audio_event_iface.h"
#include "board.h"

AudioHAL* AudioHAL::instance = nullptr;

AudioHAL* AudioHAL::get_instance() {
    if (instance == nullptr) {
        instance = new AudioHAL();
        this->board_handle = nullptr;
    }
    return instance;
}

esp_err_t AudioHAL::init() {
    esp_err_t err = ESP_OK;
    this->board_handle = audio_board_init();
    err = audio_hal_ctrl_codec(this->board_handle->audio_hal,
                               AUDIO_HAL_CODEC_MODE_DECODE,
                               AUDIO_HAL_CTRL_START);
    return err;
}