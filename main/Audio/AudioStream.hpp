//
// Created by Victor Alicino on 11/05/22.
//

#ifndef WEB_RADIO_V2_AUDIOSTREAM_H
#define WEB_RADIO_V2_AUDIOSTREAM_H

#include "audio_element.h"
#include "board.h"

enum AUDIO_CODEC{
    AAC_DECODER,
    AMR_ENCODER,
    AMR_DECODER,
    FLAC_DECODER,
    MP3_DECODER,
    OGG_DECODER,
    OPUS_DECODER,
    WAV_DECODER,
    WAV_ENCODER
};

enum AUDIO_STREAM{
    I2S_STREAM,
    HTTP_STREAM,
    FATFS_STREAM,
    RAW_STREAM,
    SPIFFS_STREAM
};

class AudioStream {
protected:
    audio_element_handle_t http_stream_reader, i2s_stream_writer, ogg_decoder;
    audio_board_handle_t board_handle;
public:
    AudioStream();
    audio_board_handle_t get_board_handle();

};


#endif //WEB_RADIO_V2_AUDIOSTREAM_H
