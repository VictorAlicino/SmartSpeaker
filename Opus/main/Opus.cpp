// ESP-IDF/ADF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "bluetooth_service.h"
#include "audio_pipeline.h"
#include "http_stream.h"
#include "mp3_decoder.h"

// Project Includes
#include "Board/Device.hpp"
#include "Board/BoardAudio.hpp"
#include "utils.cpp"
#include "Connections/A2DP_HF.hpp"
#include "Connections/ADFWiFi.hpp"
#include "AudioPipeline/AudioPipeline.hpp"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG


// Global Variables
const char* MAIN_TAG = __FILENAME__;

// Extern C Output
extern "C" {
    void app_main(void);
}

void app_main(void){
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    ESP_LOGI(MAIN_TAG, "Starting Opus");

    // ESP32 Initialization process
    if (esp32_init(ESP_LOG_DEBUG) != ESP_OK){
        ESP_LOGE(MAIN_TAG, "Failed to initialize ESP32");
        ESP_LOGE(MAIN_TAG, "Halting Opus");
        return;
    }
    ESP_LOGD(MAIN_TAG, "ESP32 Initialized, No erros found");

    // Initialize Board
    Device* Board = Device::get_instance(BOARD_TYPE::LYRAT_V4_3);
    ESP_LOGI(MAIN_TAG, "Starting Opus firmware to %s", Board->get_name().c_str());
    BoardAudio* board_audio = BoardAudio::get_instance();
    board_audio->audio_hal_init();
    ESP_LOGD(MAIN_TAG, "Hardware Audio Layer (HAL) started");

    // Initialize WiFi
    ADFWiFi* WiFi = ADFWiFi::get_instance();
    // TODO: WiFi Credentials must be personalized in some way
    std::string ssid = "SmartPTI";
    std::string password = "SmartPT12017.";
    WiFi->connect_to_wifi(ssid, password);
    ESP_LOGD(MAIN_TAG, "Connecting to WiFi, SSID: %s", ssid.c_str());

    // Initialize Bluetooth
    ESP_LOGD(MAIN_TAG, "Initializing Bluetooth");
    std::string bt_name = "Opus";
    A2DP_HF* bt_a2dp_hf = A2DP_HF::get_instance();
    bt_a2dp_hf->config(bt_name, BLUETOOTH_A2DP_SINK);
    bt_a2dp_hf->init();
    ESP_LOGD(MAIN_TAG, "Bluetooth Started, Name: %s", bt_name.c_str());

    // Creating Pipelines for each audio stream

    // Pipeline 1 (d): Bluetooth -> I2S
    // Configuring Audio Elements
    i2s_stream_cfg_t i2s_cfg1 = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg1.type = AUDIO_STREAM_WRITER;

    audio_element_handle_t bt_stream_reader = bluetooth_service_create_stream();
    audio_element_handle_t i2s_stream_writer = i2s_stream_init(&i2s_cfg1);

    // Creating Pipeline_1 (d)
    AudioPipeline* pipeline_d = new AudioPipeline(
            DEFAULT_AUDIO_PIPELINE_CONFIG(),
            "Pipeline_D");

    // Configuring Pipeline_1 (d)
    pipeline_d->register_element(bt_stream_reader, "bt");
    pipeline_d->register_element(i2s_stream_writer, "i2s_w");
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
            DEFAULT_AUDIO_PIPELINE_CONFIG(),
            "Pipeline_E");

    // Configuring Pipelines
    pipeline_e->register_element(i2s_stream_reader, "i2s_r");
    pipeline_d->register_element(raw_read, "raw");
    const char* link_e[2] = {"i2s_r", "raw"};
    pipeline_e->link_elements(link_e, 2);
    // End of Pipeline_2 (e)
/*
    // Pipeline 3 (f): HTTP Stream -> I2S
    // Configuring Audio Elements
    audio_element_handle_t http_stream_reader, i2s_stream_writer_http, mp3_decoder;
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_stream_reader = http_stream_init(&http_cfg);
    i2s_stream_cfg_t i2s_cfg3 = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg3.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer_http = i2s_stream_init(&i2s_cfg3);
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);


    // Creating Pipeline_3 (f)
    AudioPipeline* pipeline_f = new AudioPipeline(
            DEFAULT_AUDIO_PIPELINE_CONFIG(),
            "Pipeline_F");

    // Configuring Pipelines
    pipeline_f->register_element(http_stream_reader, "http");
    pipeline_f->register_element(mp3_decoder, "mp3");
    pipeline_f->register_element(i2s_stream_writer_http, "i2s_w");
    const char* link_f[3] = {"http", "mp3", "i2s_w"};
    pipeline_f->link_elements(link_f, 3);

    audio_element_set_uri(
            http_stream_reader,
            "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.mp3");
    // End of Pipeline_3 (f)
*/
    // Initializing Peripherals
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    Board->peripherals_init(&periph_cfg);

    // Initializing Touch Peripherals
    Board->touch_init();

    // Creating Bluetooth Peripheral
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    Board->start_peripheral(bt_periph);

    // Set up Event Listener
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    board_audio->audio_event_interface_init(&evt_cfg, pipeline_d);

    // Start Pipelines
    pipeline_d->run();
    pipeline_e->run();
    //pipeline_f->run();

    // A2DP Loop
    bt_a2dp_hf->start(
            board_audio,
            bt_stream_reader,
            i2s_stream_writer,
            bt_periph);
}
