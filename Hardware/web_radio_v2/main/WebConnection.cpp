//
// Created by Victor Alicino on 11/05/22.
//

#include "WebConnection.hpp"

#include "periph_wifi.h"
#include "esp_log.h"
#include "esp_peripherals.h"

WebConnection::WebConnection(): ssid(), password() {}

void WebConnection::begin(const char* ssid, const char* password) {
    this->ssid = ssid;
    this->password = password;
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    this->set = esp_periph_set_init(&periph_cfg);

    periph_wifi_cfg_t wifi_cfg = {
            .ssid = ssid,
            .password = password,
    };

    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(this->set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);
}

esp_periph_set_handle_t WebConnection::get_set() {
    return this->set;
}