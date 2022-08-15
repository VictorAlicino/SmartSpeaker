//
// Created by Victor Alicino on 19/01/22.
//

#include <Arduino.h>
#include "DigitalMic.h"

DigitalMic::DigitalMic(int serial_clock, int serial_word_select,
                       int signal_data_out, int signal_data_in) {

    i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = 16000,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
            .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0};

    i2s_pin_config_t i2s_pins = {
            .bck_io_num = serial_clock,
            .ws_io_num = serial_word_select,
            .data_out_num = signal_data_out,
            .data_in_num = signal_data_in
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
}