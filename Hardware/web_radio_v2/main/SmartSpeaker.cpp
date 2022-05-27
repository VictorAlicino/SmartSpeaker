// Project Includes
#include "Data/WebRadio.hpp"
#include "Data/WebConnection.hpp"
#include "MQTT.hpp"
#include "Startup.hpp"
// End of Project Includes

// ESP-IDF Includes
#include "sdkconfig.h"
#include "esp_log.h"
// End of ESP-IDF Includes

// Global Variables
std::string device_name;
// End of Global Variables

// Global Objects
WebRadio* Radio;
WebConnection WiFi;
MQTT* PubSub;
// End of Global objects

// Extern C Output
extern "C"{
    void app_main(void);
}
// End of Extern C Output

void app_main(void){
    init(ESP_LOG_DEBUG);

    Radio = new WebRadio();

#ifdef CONFIG_BROKER_URL
    PubSub = new MQTT(CONFIG_BROKER_URL, *Radio);
#else
    PubSub = new MQTT("mqtt://alicinotestserver:1883", *Radio);
#endif //CONFIG_BROKER_URL

    ESP_LOGD(__FILENAME__, "Cheguei aqui");

    WiFi.begin("LabIoT", "labiot2020.");

    Radio->setup_event(WiFi);

    Radio->link_to_pipeline(HTTP_STREAM, OGG_DECODER, I2S_STREAM);

    Radio->add_uri("http://alicinotestserver:8000/labiot-radio.ogg");

    Radio->run();

    Radio->loop();
}
