//
// Created by Victor Alicino on 10/10/22.
//

#include "MQTT_Client.hpp"

MQTT_Client::instance = nullptr;

MQTT_Client &MQTT_Client::getInstance() {
    if (instance == nullptr) {
        instance = new MQTT_Client();
    }
    return *instance;
}

void MQTT_Client::init() {
    ESP_LOGI(__FILENAME__, "Initializing MQTT Client");

}

void MQTT_Client::de_init() {
    ESP_LOGI(__FILENAME__, "De-Initializing MQTT Client");
}