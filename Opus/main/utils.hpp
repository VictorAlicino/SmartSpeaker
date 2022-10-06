//
// Created by Victor Alicino on 26/05/22.
//

#ifndef WEB_RADIO_V2_STARTUP_HPP
#define WEB_RADIO_V2_STARTUP_HPP

#include "esp_log.h"
#include "esp_err.h"

/**
 * @brief Initialize the ESP32
 * @param level The log level to use
 * @return ESP_OK if successful
 */
esp_err_t esp32_init(esp_log_level_t level);

#endif //WEB_RADIO_V2_STARTUP_HPP
