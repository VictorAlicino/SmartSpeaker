//
// Created by Victor Alicino on 25/05/22.
//

#ifndef WEB_RADIO_V2_MQTT_H
#define WEB_RADIO_V2_MQTT_H

#include <string>

#include "audio_element.h"
#include "mqtt_client.h"
#include "board.h"

#include "Data/WebRadio.hpp"

enum states_board{
    PLAY = 3,
    PAUSED = 4,
    STOPPED = 6
};

class MQTT {
protected:
    esp_mqtt_client_handle_t client;
    audio_board_handle_t board_handle;
public:
    MQTT(std::string server_uri, WebRadio web_radio);

    static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

};


#endif //WEB_RADIO_V2_MQTT_H
