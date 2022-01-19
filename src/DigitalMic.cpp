//
// Created by Victor Alicino on 19/01/22.
//

#include "DigitalMic.h"

DigitalMic::DigitalMic(int mck, int bck, int ws, int data_out, int data_in) {
    i2s_config_t i2s_config = {
            .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .use_apll = false,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1  // Interrupt level 1, default 0
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
}