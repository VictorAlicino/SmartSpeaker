//
// Created by Victor Alicino on 11/01/23.
//

#include "ADFWiFi.hpp"

#include "esp_netif.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "esp_check.h"
#include <string>

static const char *TAG = __FILE__;

ADFWiFi* ADFWiFi::instance = nullptr;

ADFWiFi::ADFWiFi() {
    this->ssid = nullptr;
    this->password = nullptr;
}

ADFWiFi* ADFWiFi::get_instance() {
    if (instance == nullptr) {
        instance = new ADFWiFi();
    }
    return instance;
}

esp_err_t ADFWiFi::connect_to_wifi(string ssid, string password) {
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "Initializing WiFi");
    err = esp_netif_init(); //Initialize the underlying TCP/IP stack

    ESP_LOGD(TAG, "Creating default WiFi Peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    periph_wifi_cfg_t wifi_cfg = {
            .ssid = ssid.c_str(),
            .password = password.c_str(),
    };
    ESP_LOGD(TAG, "WiFi Peripheral initializing");
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    err = esp_periph_start(set, wifi_handle);
    ESP_LOGD(TAG, "WiFi Peripheral waiting for connected status");
    err = periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);

    return err;
}

