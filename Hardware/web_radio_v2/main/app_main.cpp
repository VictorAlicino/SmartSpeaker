#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "ogg_decoder.h"
#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"
#include <array>
#include <string>
#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
#include "esp_netif.h"
#else
#include "tcpip_adapter.h"
#endif
#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#include "AudioHardware.h"
#include "WebConnection.h"

static const char *TAG = "Main Function";

std::array<std::string, 3>urls = {
        "http://alicinotestserver:8000/labiot-radio.ogg",
        "",
        ""
};

extern "C" void app_main(void){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
    ESP_ERROR_CHECK(esp_netif_init());
#else
    tcpip_adapter_init();
#endif

    AudioHardware &audioHW = AudioHardware::get_instance();
    WebConnection &webCON = WebConnection::get_instance();

    //Register elements to audio pipeline
    audio_pipeline_register(audioHW.get_pipeline(), webCON.get_http_stream_reader(), "http");
    audio_pipeline_register(audioHW.get_pipeline(), audioHW.get_ogg_decoder(), "ogg");
    audio_pipeline_register(audioHW.get_pipeline(), audioHW.get_i2s_streamer_writer(), "i2s");

    //Link it together http_stream-->ogg_decoder-->i2s_stream-->[codec_chip]
    const char *link_tag[3] = {"http", "ogg", "i2s"};
    audio_pipeline_link(audioHW.get_pipeline(), &link_tag[0], 3);

    //Setup radio URI
    audio_element_set_uri(webCON.get_http_stream_reader(), urls.at(0).c_str());

    //Start and wai for Wi-Fi
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    periph_wifi_cfg_t wifi_cfg = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);
}
