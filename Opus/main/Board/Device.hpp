//
// Created by Victor Alicino on 26/05/22.
//

#ifndef SMART_SPEAKER_DEVICE_H
#define SMART_SPEAKER_DEVICE_H

//TODO: Change to singleton class

#include "audio_event_iface.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"

#include <string>
#include <map>

// Board hardware buttons
typedef struct BoardButtonsIDs{
    int8_t play;
    int8_t set;
    int8_t mode;
    int8_t volup;
    int8_t voldown;
    int8_t rec;
} BoardButtons;

// Board hardware LEDs
typedef struct BoardLEDIDs{
    int8_t green;
    int8_t blue;
} BoardLEDs;

// Board type
enum BOARD_TYPE{
    LYRAT_V4_3,
    WROOM32
};

class Device{
private:
    std::string code;
    std::string name;
    std::map<std::string, esp_periph_set_handle_t> esp_peripherals;

public:
    /**
     * Class constructor
     * @param board Board model
     */
    Device(BOARD_TYPE board);

    // Public variables
    BoardButtons button;
    BoardLEDs led;

    /**
     * Get the full name of the board
     * @return STD String containing the board name + the board code
     */
    std::string get_full_name();

     /**
      * Get the name of the board
      * @return STD String containing the board name
      */
    std::string get_name();

    /**
     * Get the code of the board
     * @return STD String containing the board code
     */
    std::string get_code();

    /**
     * Initialize board peripherals
     * @param set_name Peripheral set instance name
     * @param config Peripheral configuration
     * @return The peripheral sets instance
     */
    esp_periph_set_handle_t peripheral_init(std::string set_name, esp_periph_config_t config);

    /**
     * Get the peripheral set instance
     * @param set_name Peripheral set instance name
     * @return The peripheral sets instance
     */
    esp_periph_set_handle_t get_peripheral_handle(std::string set_name);

    /**
     * Checks if event from iface is a button press
     * @param msg Event message
     * @return True if is a button press, false otherwise
     */
    bool is_board_button_event(audio_event_iface_msg_t &msg);

    /**
     * Get the MAC address of the board
     * @return STD String containing the MAC address
     */
    std::string get_mac_address();
};

#endif //SMART_SPEAKER_DEVICE_H