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

Device::Device() {
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