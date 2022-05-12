#include "nvs_flash.h"

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

#include "AudioPipeline.hpp"
#include "AudioStream.hpp"
#include "WebConnection.hpp"

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

    AudioStream Stream = AudioStream();
    WebConnection WiFi = WebConnection();
    AudioPipeline Pipeline = AudioPipeline();

    Pipeline.register_to_pipeline(Stream.get_http_stream_reader(), "http");
    Pipeline.register_to_pipeline(Stream.get_ogg_decoder(), "ogg");
    Pipeline.register_to_pipeline(Stream.get_i2s_stream_writer(), "i2s");

    Pipeline.link_to_pipeline();

    Pipeline.add_uri("http://alicinotestserver:8000/labiot-radio.ogg");

    WiFi.begin("LabIoT", "labiot2020.");

    Pipeline.setup_event(WiFi);

    Pipeline.loop(Stream);
}
