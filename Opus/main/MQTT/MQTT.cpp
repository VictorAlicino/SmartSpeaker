//
// Created by Victor Alicino on 25/05/22.
//

#include "MQTT.hpp"

#include <string>

#include "esp_log.h"

const char* MQTT_TAG = __FILENAME__;

//TODO: Check for possible erros in here

//extern WebRadio *Radio;

MQTT* MQTT::instance = nullptr; //Defining Singleton instance as null

MQTT::MQTT() {}

MQTT* MQTT::get_instance() {
    if (instance == nullptr) {
        instance = new MQTT();
    }
    return instance;
}

void MQTT::init(void (*event_handler)(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data),
                const char *uri) {
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    esp_mqtt_client_config_t mqtt_cfg = {
            .uri = uri,
    };
#pragma GCC diagnostic pop
    this->client = esp_mqtt_client_init(&mqtt_cfg);

    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(this->client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID,
                                   event_handler, (void *) 0);
    esp_mqtt_client_start(this->client);
}

void MQTT::de_init() {
    esp_mqtt_client_stop(this->client);
    esp_mqtt_client_destroy(this->client);
    delete this->instance;
}
