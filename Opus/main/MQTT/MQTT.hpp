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
private:
    /**
    * Class constructor
    */
    MQTT();

    static MQTT* instance; // MQTT Singleton instance

public:
    /**
     * MQTT get Singleton instance
     * @return MQTT Instance
     */
    static MQTT* getInstance();

    /**
     * MQTT client
     * @param event_handler Class specific MQTT event handler
     */
    void init(void (*event_handler)(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data),
              const char *uri);

    /**
     * MQTT de init
     */
    void de_init();

    esp_mqtt_client_handle_t client; // MQTT client handle
};

#endif //SMART_SPEAKER_MQTT_H
