//
// Created by Victor Alicino on 26/05/22.
//

#include "Device.hpp"

#include "esp_peripherals.h"
#include "periph_touch.h"
#include "esp_log.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include <string>
#include <stdexcept>

const char* DEVICE_TAG = __FILENAME__;

Device::Device(BOARD_TYPE board) {
    if (board == LYRAT_V4_3) {
        this->button = {
                .play = get_input_play_id(),
                .set = get_input_set_id(),
                .mode = get_input_mode_id(),
                .volup = get_input_volup_id(),
                .voldown = get_input_voldown_id(),
                .rec = get_input_rec_id()
        };

        this->led = {
                .green = get_green_led_gpio(),
                .blue = 0,
        };
    }

    if (board == WROOM32){
        this->led = {
                .green = GPIO_NUM_2,
                .blue = nullptr
        };
    }
}

std::string Device::get_full_name() {
    std::string full_name;
    full_name = this->name + " (" + 
                this->code + ")";
    return full_name;
}

std::string Device::get_name() {
    return this->name;
}

std::string Device::get_code() {
    return this->code;
}

esp_periph_set_handle_t Device::peripheral_init(std::string set_name, esp_periph_config_t config){
    ESP_LOGI(DEVICE_TAG, "Initialize peripheral: %s", set_name.c_str());
    esp_periph_set_handle_t set = esp_periph_set_init(&config);

    this->esp_peripherals[set_name] = set;
    ESP_LOGI(DEVICE_TAG, "%s initialized", set_name.c_str());

    return set;
}

esp_periph_set_handle_t Device::get_peripheral_handle(std::string set_name) {
    auto it = this->esp_peripherals.find(set_name);
    if(it->first != set_name){
        throw std::invalid_argument("Provided set name does not match with initialized sets");
    }
    return it->second;
}

bool Device::is_board_button_event(audio_event_iface_msg_t &msg){
    if ((msg.source_type == PERIPH_ID_TOUCH
        || msg.source_type == PERIPH_ID_BUTTON
        || msg.source_type == PERIPH_ID_ADC_BTN)
    &&
        (msg.cmd == PERIPH_TOUCH_TAP
        || msg.cmd == 0
        || msg.cmd == PERIPH_BUTTON_PRESSED
        || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) { return true; }
    else { return false; }
}

std::string Device::get_mac_address() {
    //Get mac address out of ESP32 using ESP-IDF API
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(mac_str);
}

std::string Device::get_ip_address() {
    //Get IP address out of ESP32 using ESP-IDF API
    char ip_str[16];
    sprintf(ip_str, IPSTR, IP2STR(&ip4_addr));
    return std::string(ip_str);
}

std::string get_gateway() {
    //Get gateway out of ESP32 using ESP-IDF API
    char gw_str[16];
    sprintf(gw_str, IPSTR, IP2STR(&gw4_addr));
    return std::string(gw_str);
}