// ESP-IDF/ADF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "bluetooth_service.h"
#include "audio_pipeline.h"

// Project Includes
#include "Board/Device.hpp"
#include "Board/BoardAudio.hpp"
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
    BoardAudio* board_audio = BoardAudio::get_instance();
    board_audio->init();
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

    // Creating Pipelines for each audio stream

    // Pipeline 1 (d): Bluetooth -> I2S
    // Configuring Audio Elements
    i2s_stream_cfg_t i2s_cfg1 = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg1.type = AUDIO_STREAM_WRITER;

    audio_element_handle_t bt_stream_reader = bluetooth_service_create_stream();
    audio_element_handle_t i2s_stream_writer = i2s_stream_init(&i2s_cfg1);

    // Creating Pipeline_1 (d)
    AudioPipeline* pipeline_d = new AudioPipeline(
            DEFAULT_AUDIO_PIPELINE_CONFIG());

    // Configuring Pipeline_1 (d)
    pipeline_d->register_element(bt_stream_reader, "bt");
    pipeline_d->register_element(i2s_stream_writer, "i2s");
    const char* link_d[2] = {"bt", "i2s_w"};
    pipeline_d->link_elements(link_d, 2);
    // End of Pipeline_1 (d)

    // Pipeline 2 (e): I2S -> Bluetooth
    // Configuring Audio Elements
    i2s_stream_cfg_t i2s_cfg2 = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg2.type = AUDIO_STREAM_READER;
    raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
    raw_cfg.type = AUDIO_STREAM_READER;

    audio_element_handle_t i2s_stream_reader = i2s_stream_init(&i2s_cfg2);
    audio_element_handle_t raw_read = raw_stream_init(&raw_cfg);

    // Creating Pipeline_2 (e)
    AudioPipeline* pipeline_e = new AudioPipeline(
            DEFAULT_AUDIO_PIPELINE_CONFIG());

    // Configuring Pipelines
    pipeline_e->register_element(i2s_stream_reader, "i2s_r");
    pipeline_d->register_element(raw_read, "raw");
    const char* link_e[2] = {"i2s_r", "raw"};
    pipeline_e->link_elements(link_e, 2);
    // End of Pipeline_2 (e)

    // Initializing Peripherals
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    Board->peripherals_init(&periph_cfg);

    // Initializing Touch Peripherals
    Board->touch_init();

    // Creating Bluetooth Peripheral
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    Board->start_peripheral(bt_periph);


}
