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

#include "Data/WebRadio.hpp"
#include "Data/WebConnection.hpp"

#include "esp_log.h"

extern "C"{
    void app_main(void);
}

void app_main(void){
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

    WebRadio Radio;
    WebConnection WiFi;

    Radio.register_to_pipeline(Radio.get_http_stream_reader(), "http");
    Radio.register_to_pipeline(Radio.get_ogg_decoder(), "ogg");
    Radio.register_to_pipeline(Radio.get_i2s_stream_writer(), "i2s");

    Radio.link_to_pipeline();

    Radio.add_uri("http://alicinotestserver:8000/labiot-radio.ogg");

    WiFi.begin("LabIoT", "labiot2020.");

    Radio.setup_event(WiFi);

    Radio.run();

    Radio.loop();
}
