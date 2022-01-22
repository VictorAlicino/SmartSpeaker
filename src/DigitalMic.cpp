//
// Created by victo on 22/01/2022.
//

#include "DigitalMic.h"
#include "driver/i2s.h"
#include "freertos/queue.h"

DigitalMic::DigitalMic() {
    i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = 40000,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
    };

}