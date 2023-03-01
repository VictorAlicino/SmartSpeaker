//
// Created by victo on 27/01/2023.
//

#include "BoardAudio.hpp"

#include "audio_hal.h"
#include "audio_event_iface.h"
#include "board.h"
#include "Device.hpp"
#include "AudioPipeline/AudioPipeline.hpp"
#include "esp_log.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

const char* BOARD_AUDIO_TAG = "BoardAudio";

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

esp_err_t BoardAudio::audio_hal_init() {
    ESP_LOGD(BOARD_AUDIO_TAG, "Initializing AudioHAL");
    esp_err_t err = ESP_OK;
    this->board_handle = audio_board_init();
    err = audio_hal_ctrl_codec(this->board_handle->audio_hal,
                               AUDIO_HAL_CODEC_MODE_DECODE,
                               AUDIO_HAL_CTRL_START);
    if(err != ESP_OK) ESP_LOGD(BOARD_AUDIO_TAG, "Error initializing AudioHAL");
    return err;
}

esp_err_t BoardAudio::audio_event_interface_init(audio_event_iface_cfg_t* cfg, AudioPipeline* pipeline) {
    esp_err_t err = ESP_OK;
    Device *board = Device::get_instance(BOARD_TYPE::LYRAT_V4_3);
    this->evt_handle = audio_event_iface_init(cfg);

    err = audio_pipeline_set_listener(
            pipeline->get_pipeline(),
            this->evt_handle
            );

    if (err != ESP_OK) return err;

    err = audio_event_iface_set_listener(
            esp_periph_set_get_event_iface(board->get_peripherals_handle()),
            this->evt_handle
            );

    return err;
}

audio_board_handle_t BoardAudio::get_board_handle() {
    return this->board_handle;
}

audio_event_iface_handle_t BoardAudio::get_evt_handle() {
    return this->evt_handle;
}