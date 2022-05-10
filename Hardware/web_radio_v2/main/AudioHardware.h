//
// Created by Victor Alicino on 10/05/22.
//

#ifndef WEB_RADIO_V2_AUDIOHARDWARE_H
#define WEB_RADIO_V2_AUDIOHARDWARE_H

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"


class AudioHardware {
private:
    //ESP-ADF hardware handlers
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_streamer_writer, ogg_decoder;
    audio_board_handle_t board_handle;

    //Singleton constructor
    AudioHardware();


public:
    static AudioHardware& get_instance();

    //Getters
    audio_pipeline_handle_t get_pipeline() const;
    audio_element_handle_t get_i2s_streamer_writer() const;
    audio_element_handle_t get_ogg_decoder() const;
    audio_board_handle_t get_board_handle() const;
};


#endif //WEB_RADIO_V2_AUDIOHARDWARE_H
