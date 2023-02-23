//
// Created by victo on 18/01/2023.
//

#ifndef SMARTSPEAKER_A2DP_HF_HPP
#define SMARTSPEAKER_A2DP_HF_HPP

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_client_api.h"

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_mem.h"

#include "i2s_stream.h"
#include "board.h"
#include "bluetooth_service.h"
#include "filter_resample.h"
#include "raw_stream.h"

#include "AudioPipeline/AudioPipeline.hpp"
#include <string>
#include <cstring>


class A2DP_HF {
private:
    /**
     * @brief    Class constructor
     */
    A2DP_HF();

    static A2DP_HF* instance; // A2DP_HF Singleton instance
    bluetooth_service_cfg_t bt_cfg; // Bluetooth service config
    audio_element_handle_t bt_stream_reader; // Bluetooth stream reader
    audio_element_handle_t raw_read; // Raw reader


public:
    static A2DP_HF* get_instance();

    void config(std::string device_name,
                bluetooth_service_mode_t mode);

    void init();

    esp_err_t bt_stream_reader_init();
    esp_err_t raw_read_init();

    audio_element_handle_t get_bt_stream_reader();
    audio_element_handle_t get_raw_read();

};

void bluetooth_service_hf_client_cb(
        esp_hf_client_cb_event_t event,
        esp_hf_client_cb_param_t *param);

//void pipeline_cfg_to_a2dp_reader(AudioPipeline *pipeline);

//void pipeline_cfg_to_a2dp_writer(AudioPipeline *pipeline);

#endif //SMARTSPEAKER_A2DP_HF_HPP
