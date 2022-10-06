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

    /**
     * Event handler for MQTT events
     */
    void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

    /**
     * MQTT Callback when connected to server
     */
    void mqtt_connected();

    /**
     * MQTT Callback when disconnected from server
     */
    void mqtt_disconnected();

    /**
     * MQTT Callback when subscribed to topic
     */
    void mqtt_subscribed();

    /**
     * MQTT Callback when unsubscribed from topic
     */
    void mqtt_unsubscribed();

    /**
     * MQTT Callback when publish
     */
    void mqtt_published();

    /**
     * MQTT Callback when data received
     * @param event MQTT event
     */
    void mqtt_data(esp_mqtt_event_handle_t event);

    /**
     * MQTT Callback when an error is generated
     */
    void mqtt_error();

};

#endif //SMART_SPEAKER_MQTT_H
