// Project Includes
#include "Board/Device.hpp"
#include "utils.cpp"

// ESP-IDF Includes
#include "sdkconfig.h"
#include "esp_log.h"

// Global Objects
Device* Board;

// Global Variables
const char* MAIN_TAG = __FILENAME__;

// Extern C Output
extern "C" {
    void app_main(void);
}

void app_main(void){
    ESP_LOGI(MAIN_TAG, "Starting Opus");

    // ESP32 Initialization process
    if (esp32_init(ESP_LOG_DEBUG) != ESP_OK){
        ESP_LOGE(MAIN_TAG, "Failed to initialize ESP32");
        ESP_LOGE(MAIN_TAG, "Halting Opus");
        return;
    }
    ESP_LOGD(__FILENAME__, "ESP32 Initialized, No erros found");

    Board = new Device(LYRAT_V4_3);
    ESP_LOGD(__FILENAME__, "Starting Opus firmware to %s", Board->get_name().c_str());

    //TODO: AudioPipeline
}
