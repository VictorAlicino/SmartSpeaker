//
// Created by Victor Alicino on 11/05/22.
//

#ifndef WEB_RADIO_V2_WEBCONNECTION_H
#define WEB_RADIO_V2_WEBCONNECTION_H

#include <string>
#include "esp_peripherals.h"

class WebConnection {
private:
    std::string ssid;
    std::string password;
    esp_periph_handle_t set;

public:
    WebConnection();
    void begin(const std::string &ssid, const std::string &password);
    esp_periph_handle_t get_set();

};


#endif //WEB_RADIO_V2_WEBCONNECTION_H
