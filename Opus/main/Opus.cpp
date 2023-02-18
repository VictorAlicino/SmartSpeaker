// ESP-IDF/ADF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "bluetooth_service.h"
#include "audio_pipeline.h"

// Project Includes
#include "Board/Device.hpp"
#include "Board/AudioHAL.hpp"
#include "utils.cpp"
#include "Connections/A2DP_HF.hpp"
#include "Connections/ADFWiFi.hpp"
#include "AudioPipeline/AudioPipeline.hpp"


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

    // Initialize Board
    Device* Board = Device::get_instance(BOARD_TYPE::LYRAT_V4_3);
    ESP_LOGI(__FILENAME__, "Starting Opus firmware to %s", Board->get_name().c_str());
    AudioHAL* audio_hal = AudioHAL::get_instance();
    audio_hal->init();
    ESP_LOGD(__FILENAME__, "Hardware Audio Layer (HAL) starterd");


    // Initialize WiFi
    ADFWiFi* WiFi = ADFWiFi::get_instance();
    // TODO: WiFi Credentials must be personalized in some way
    std::string ssid, password;
    ssid = "whatever";
    password = "whatever";
    WiFi->connect_to_wifi(ssid, password);
    ESP_LOGD(__FILENAME__, "Connecting to WiFi, SSDI: %s", ssid.c_str());

    // Initialize Bluetooth
    A2DP_HF* bt_a2dp_hf = A2DP_HF::get_instance();
    bt_a2dp_hf->config("Opus", BLUETOOTH_A2DP_SINK);
    bt_a2dp_hf->init();

    //TODO: AudioPipeline
    // Creating Pipelines for each audio stream
    AudioPipeline* pipeline_d = new AudioPipeline(DEFAULT_AUDIO_PIPELINE_CONFIG());
    AudioPipeline* pipeline_e = new AudioPipeline(DEFAULT_AUDIO_PIPELINE_CONFIG());

}
