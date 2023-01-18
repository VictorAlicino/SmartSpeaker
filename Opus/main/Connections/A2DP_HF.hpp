//
// Created by victo on 18/01/2023.
//

#ifndef SMARTSPEAKER_A2DP_HF_HPP
#define SMARTSPEAKER_A2DP_HF_HPP

#include "bluetooth_service.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_client_api.h"
#include <string>


class A2DP_HF {
private:
    /**
     * @brief    Class constructor
     */
    A2DP_HF();

    static A2DP_HF* instance; // A2DP_HF Singleton instance
    bluetooth_service_cfg_t bt_cfg; // Bluetooth service config


public:
    static A2DP_HF* get_instance();

    void config(std::string device_name,
                std::string remote_name,
                bluetooth_service_mode_t mode);

    void init();

};

void bluetooth_service_hf_client_cb(
        esp_hf_client_cb_event_t event,
        esp_hf_client_cb_param_t *param);


#endif //SMARTSPEAKER_A2DP_HF_HPP
