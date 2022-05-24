//
// Created by Victor Alicino on 11/05/22.
//

#ifndef WEB_RADIO_V2_AUDIOSTREAM_H
#define WEB_RADIO_V2_AUDIOSTREAM_H

#include "audio_element.h"
#include "board.h"


class AudioStream {
protected:
    audio_element_handle_t http_stream_reader, i2s_stream_writer, ogg_decoder;
    audio_board_handle_t board_handle;
public:
    AudioStream();
    audio_element_handle_t get_http_stream_reader();
    audio_element_handle_t get_i2s_stream_writer();
    audio_element_handle_t get_ogg_decoder();
    audio_board_handle_t get_board_handle();

};


#endif //WEB_RADIO_V2_AUDIOSTREAM_H
