//
// Created by Victor Alicino on 11/01/23.
//

#ifndef OPUS_ADFWIFI_H
#define OPUS_ADFWIFI_H

#include "esp_check.h"
#include <string>


class ADFWiFi {
private:
    //Singleton instance
    static ADFWiFi* instance;

    std::string ssid;
    std::string password;

    /**
     * @brief Class constructor
     */
    ADFWiFi();
public:

    /**
     * @brief Get singleton instance
     * @return Singleton instance
     */
    static ADFWiFi* get_instance();

    /**
     * @brief Connect to WiFi
     * @param ssid WiFi SSID
     * @param password WiFi password
     * @return ESP_OK if successful
     */
    esp_err_t connect_to_wifi(std::string ssid, std::string password);

};


#endif //OPUS_ADFWIFI_H
