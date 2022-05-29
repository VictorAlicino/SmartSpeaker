//
// Created by Victor Alicino on 26/05/22.
//

#include "Startup.hpp"
#include "Device.hpp"

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

#include "nvs_flash.h"
#include "esp_log.h"

extern Device* Board;

const char* STARTUP_TAG;

esp_err_t init(esp_log_level_t level){
    ESP_LOGI(STARTUP_TAG, "Board fullname: %s", *Board->get_full_name().c_str());
    esp_log_level_set("*", level);

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
    return ESP_OK;
}
