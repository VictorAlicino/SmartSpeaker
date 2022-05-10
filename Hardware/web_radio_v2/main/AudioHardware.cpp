//
// Created by Victor Alicino on 10/05/22.
//

#include "AudioHardware.h"

AudioHardware::AudioHardware() {
    //Starting CODEC chip
    this->board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    //Creating audio pipeline for playback
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    this->pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    //Creating I2S stream to write data to CODEC chip
    i2s_stream_cfg_t i2s_cfg = i2s_stream_init(&i2s_cfg);

    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    //Create the OGG DECODER to decode OGG File
    ogg_decoder_cfg_t ogg_cfg = DEFAULT_OGG_DECODER_CONFIG();
    ogg_decoder = ogg_decoder_init(&ogg_cfg);
}

static AudioHardware& AudioHardware::get_instance() {
    static AudioHardware me;
    return me;
}

audio_pipeline_handle_t AudioHardware::get_pipeline() {
    return this->pipeline;
}

audio_element_handle_t AudioHardware::get_i2s_streamer_writer() {
    return this->i2s_streamer_writer;
}

audio_element_handle_t AudioHardware::get_ogg_decoder() {
    return this->ogg_decoder;
}

audio_board_handle_t AudioHardware::get_board_handle() {
    return this->board_handle;
}