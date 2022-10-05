//
// Created by Victor Alicino on 25/05/22.
//

#ifndef SMART_SPEAKER_MQTT_H
#define SMART_SPEAKER_MQTT_H

#include <string>

#include "audio_element.h"
#include "mqtt_client.h"
#include "board.h"

class MQTT {
protected:
    esp_mqtt_client_handle_t client;
public:

    /**
     * Class constructor
     * @param server_uri MQTT server URI
     */
    MQTT(std::string server_uri);

    static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

};


#endif //SMART_SPEAKER_MQTT_H
