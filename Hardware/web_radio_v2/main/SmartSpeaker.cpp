// Project Includes
#include "Audio/WebRadio.hpp"
#include "Audio/WebConnection.hpp"
#include "MQTT.hpp"
#include "Device.hpp"
#include "Startup.hpp"
// End of Project Includes

// ESP-IDF Includes
#include "sdkconfig.h"
#include "esp_log.h"
// End of ESP-IDF Includes

// General Includes
#include <string>
//End of General Includes

// Global Variables
    // Nothing here
// End of Global Variables

// Global Objects
Device* Board;
WebRadio* Radio;
WebConnection WiFi;
MQTT* PubSub;
// End of Global objects

// Extern C Output
extern "C"{
    void app_main(void);
}
// End of Extern C Output

// Main Function
void app_main(void){
    Board = new Board();

    init(ESP_LOG_DEBUG);

    Radio = new WebRadio();

#ifdef CONFIG_BROKER_URL
    PubSub = new MQTT(CONFIG_BROKER_URL, *Radio);
#else
    PubSub = new MQTT("mqtt://alicinotestserver:1883", *Radio);
#endif //CONFIG_BROKER_URL

#ifdef CONFIG_WIFI_SSID
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
#else
    WiFi.begin("LabIoT", "labiot2020.");
#endif //CONFIG_WIFI_SSID

    ESP_LOGD(__FILENAME__, "Cheguei aqui");

    WiFi.begin("LabIoT", "labiot2020.");

    Radio->setup_event(WiFi);

    Radio->link_to_pipeline(HTTP_STREAM, OGG_DECODER, I2S_STREAM);

    Radio->add_uri("http://alicinotestserver:8000/labiot-radio.ogg");

    Radio->run();

    Radio->loop();
}
