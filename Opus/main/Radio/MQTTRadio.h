//
// Created by Victor Alicino on 17/10/22.
//

#ifndef OPUS_MQTTRADIO_H
#define OPUS_MQTTRADIO_H

#include "mqtt_client.h"
#include "MQTT/MQTT.hpp"

void radio_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

class MQTTRadio {
public:
    /**
     * MQTT Callback when connected to server
     */
    void _mqtt_connected();

    /**
     * MQTT Callback when disconnected from server
     */
    void _mqtt_disconnected();

    /**
     * MQTT Callback when subscribed to topic
     */
    void _mqtt_subscribed();

    /**
     * MQTT Callback when unsubscribed from topic
     */
    void _mqtt_unsubscribed();

    /**
     * MQTT Callback when publish
     */
    void _mqtt_published();

    /**
     * MQTT Callback when data received
     * @param event MQTT event
     */
    void _mqtt_data(esp_mqtt_event_handle_t event);

    /**
     * MQTT Callback when an error is generated
     */
    void _mqtt_error(esp_mqtt_event_handle_t event);
};


#endif //OPUS_MQTTRADIO_H
