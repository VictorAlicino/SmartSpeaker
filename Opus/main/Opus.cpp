// ESP-IDF/ADF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "bluetooth_service.h"

// Project Includes
#include "Board/Device.hpp"
#include "utils.cpp"
#include "Connections/A2DP_HF.hpp"
#include "Connections/ADFWiFi.hpp"


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

    Device* Board = Device::get_instance(BOARD_TYPE::LYRAT_V4_3);
    ESP_LOGD(__FILENAME__, "Starting Opus firmware to %s", Board->get_name().c_str());

    // Initialize WiFi
    ADFWiFi* WiFi = ADFWiFi::get_instance();
    WiFi->connect_to_wifi("whatever", "whatever");

    // Initialize Bluetooth
    A2DP_HF* bluetooth = A2DP_HF::get_instance();
    bluetooth->config("Opus", BLUETOOTH_A2DP_SINK);
    bluetooth->init();

    //TODO: AudioPipeline

}
