//
// Created by Victor Alicino on 25/05/22.
//

#ifndef SMART_SPEAKER_MQTT_H
#define SMART_SPEAKER_MQTT_H

#include <string>

#include "audio_element.h"
#include "mqtt_client.h"
#include "board.h"

#include "Audio/WebRadio.hpp"

class MQTT {
protected:
    esp_mqtt_client_handle_t client;
    audio_board_handle_t board_handle;
public:
    MQTT(std::string server_uri, WebRadio web_radio);

    static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

};


#endif //SMART_SPEAKER_MQTT_H
