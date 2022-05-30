//
// Created by Victor Alicino on 26/05/22.
//

#ifndef SMART_SPEAKER_DEVICE_H
#define SMART_SPEAKER_DEVICE_H

#include "audio_event_iface.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"

#include <string>
#include <map>

typedef struct BoardButtonsIDs{
    int8_t play,
    int8_t set,
    int8_t mode,
    int8_t volup,
    int8_t voldown,
    int8_t rec
} BoardButtons;

typedef struct BoardLEDIDs{
    int8_t green,
    int8_t blue
} BoardLEDs;

class Device{
private:
    std::string code;
    std::string name;
    std::map<std::string, esp_periph_set_handle_t> esp_peripherals;

public:
    Device();
    BoardButtons button;
    BoardLEDs led;
    std::string get_full_name;
    std::string get_name;
    std::string get_code;
    esp_periph_set_handle_t peripheral_init(std::string set_name, esp_periph_config_t config);
    esp_periph_set_handle_t get_peripheral_handle(std::string set_name);
    bool is_board_button_event(audio_event_iface_msg_t &msg);
}

#endif //SMART_SPEAKER_DEVICE_H