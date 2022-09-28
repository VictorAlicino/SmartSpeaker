// Project Includes
#include "Audio/WebRadio.hpp"
#include "Audio/WebConnection.hpp"
#include "MQTT.hpp"
#include "Device.hpp"
#include "Startup.hpp"

// ESP-IDF Includes
#include "sdkconfig.h"
#include "esp_log.h"

// General Includes
#include <string>

// Global Objects
Device* Board;
WebRadio* Radio;
WebConnection WiFi;
MQTT* PubSub;

// Extern C Output
extern "C"{
    void app_main(void);
}

// Main Function
void app_main(void){
    Board = new Device();

    init(ESP_LOG_DEBUG); //ESP Log initializer

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

    Radio->setup_event(WiFi); //Setup WiFi
    Radio->link_to_pipeline(HTTP_STREAM, OGG_DECODER, I2S_STREAM); //Build the pipeline elements in order
    Radio->add_uri("http://alicinotestserver:8000/labiot-radio.ogg"); //Add a new Radio URI
    Radio->run();
    Radio->loop();
}
