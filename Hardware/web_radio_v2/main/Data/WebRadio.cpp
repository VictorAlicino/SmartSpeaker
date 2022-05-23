//
// Created by Victor Alicino on 23/05/22.
//

#include "WebRadio.hpp"
#include "esp_log.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "i2s_stream.h"


#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"

esp_err_t WebRadio::add_uri(std::string url) {
    this->urls.push_back(url);
    this->url_available = true;
    ESP_LOGI(__FILENAME__, "\nAdded URI:\n%s\nTotal URIs registered:%d", url.c_str(), this->urls.size());
    ESP_LOGD(__FILENAME__, "The follow URL are available:");
    if(esp_log_level_get(__FILENAME__) == ESP_LOG_DEBUG){
        for(int i=0; i < this->urls.size(); i++) {
            printf("%s\n", urls.at(i).c_str());
        }
    }
    audio_element_set_uri(get_http_stream_reader(), urls.at(0).c_str());
    return ESP_OK;
}

esp_err_t WebRadio::loop() {
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    int player_volume;
    audio_hal_get_volume(board_handle->audio_hal, &player_volume);
    ESP_LOGD(__FILENAME__, "AUDIO HAL GET VOLUME = %d", player_volume);

    while(this->activated){
        ESP_LOGI(__FILENAME__, "Volume at = %d", player_volume);
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(this->evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(__FILENAME__, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
            && msg.source == (void *) get_ogg_decoder()
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {
                    0
            };
            audio_element_getinfo(get_ogg_decoder(), &music_info);

            ESP_LOGI(__FILENAME__, "[ * ] Receive music info from ogg decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(get_i2s_stream_writer(), &music_info);
            i2s_stream_set_clk(get_i2s_stream_writer(), music_info.sample_rates,
                               music_info.bits, music_info.channels);
            continue;
        }

        if(gpio_get_level(GPIO_NUM_13) == 1) {
            ESP_LOGI(__FILENAME__, "[ * ] [Vol-] touch tap event");
            player_volume -= 10;
            if (player_volume < 0) {
                player_volume = 0;
                audio_hal_set_volume(board_handle->audio_hal, player_volume);
                ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
            }
        }

        if(gpio_get_level(GPIO_NUM_27) == 1){
            ESP_LOGI(__FILENAME__, "[ * ] [Vol+] touch tap event");
            player_volume += 10;
            if (player_volume > 100) {
                player_volume = 100;
            }
            audio_hal_set_volume(board_handle->audio_hal, player_volume);
            ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
        }

        // Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) get_i2s_stream_writer()
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(__FILENAME__, "[ * ] Stop event received");
            break;
        }
/*
        if ((int) msg.data == get_input_set_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [Set] touch tap event");
                ESP_LOGI(__FILENAME__, "[ * ] Stopping audio pipeline");
                break;
            } else if ((int) msg.data == get_input_mode_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [mode] tap event");
                audio_pipeline_stop(pipeline);
                audio_pipeline_wait_for_stop(pipeline);
                audio_pipeline_terminate(pipeline);
                audio_pipeline_reset_ringbuffer(pipeline);
                audio_pipeline_reset_elements(pipeline);
                audio_pipeline_run(pipeline);
            } else if ((int) msg.data == get_input_volup_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [Vol+] touch tap event");
                player_volume += 10;
                if (player_volume > 100) {
                    player_volume = 100;
                }
                audio_hal_set_volume(board_handle->audio_hal, player_volume);
                ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
            } else if ((int) msg.data == get_input_voldown_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [Vol-] touch tap event");
                player_volume -= 10;
                if (player_volume < 0) {
                    player_volume = 0;
                audio_hal_set_volume(board_handle->audio_hal, player_volume);
                ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
            }
        }*/

    }
    return ESP_OK;
}