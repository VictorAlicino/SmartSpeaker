//
// Created by Victor Alicino on 25/05/22.
//

#ifndef SMART_SPEAKER_MQTT_H
#define SMART_SPEAKER_MQTT_H

#include <string>

#include "audio_element.h"
#include "board.h"

class MQTT {
private:
    /**
    * Class constructor
    */
    MQTT();

    static MQTT_Client* instance; // MQTT Singleton instance

    /**
    * Event handler for MQTT events
    */
    void _event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

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
    void _mqtt_error();

    esp_mqtt_client_handle_t client; // ESP-MQTT Client
public:
    /**
     * MQTT get Singleton instance
     * @return MQTT Instance
     */
    static MQTT &getInstance();

    /**
     * MQTT init
     * @param mqtt_cfg ESP-MQTT Client Config struct
     */
    void init(esp_mqtt_client_config_t mqtt_cfg);
    void de_init();
};

#endif //SMART_SPEAKER_MQTT_H
