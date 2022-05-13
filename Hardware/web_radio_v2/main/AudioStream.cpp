//
// Created by Victor Alicino on 11/05/22.
//

#include "AudioStream.hpp"
#include "esp_log.h"
#include "board.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "ogg_decoder.h"

AudioStream::AudioStream() {
    ESP_LOGD(__FILENAME__, "Starting audio CODEC chip                             [AudioStream: 1/4]");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal,
                         AUDIO_HAL_CODEC_MODE_DECODE,
                         AUDIO_HAL_CTRL_START);
    ESP_LOGI(__FILENAME__, "Audio CODEC chip successfully started");

    ESP_LOGD(__FILENAME__, "Creating HTTP Stream to READ data                     [AudioStream: 2/4]")
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    this->http_stream_reader = http_stream_init(&http_cfg);
    ESP_LOGI(__FILENAME__, "HTTP data input stream created");

    ESP_LOGD(__FILENAME__, "Initializing I2S stream to WRITE data to CODEC chip   [AudioStream: 3/4]");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();

    i2s_cfg.type = AUDIO_STREAM_WRITER;
    this->i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    ESP_LOGI(__FILENAME__, "I2S data output stream initialized");

    ESP_LOGD(__FILENAME__, "Creating an OGG decoder to decode the OGG stream      [AudioStream: 4/4]");
    ogg_decoder_cfg_t ogg_cfg = DEFAULT_OGG_DECODER_CONFIG();
    this->ogg_decoder = ogg_decoder_init(&ogg_cfg);
    ESP_LOGI(__FILENAME__, "OGG decoder created");


}

audio_element_handle_t AudioStream::get_http_stream_reader() {
    return this->http_stream_reader;
}

audio_element_handle_t AudioStream::get_i2s_stream_writer() {
    return this->i2s_stream_writer;
}

audio_element_handle_t AudioStream::get_ogg_decoder() {
    return this->ogg_decoder;
}