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
#include "MQTT.hpp"

extern int volume;
extern int board_state;

esp_err_t WebRadio::add_uri(std::string url) {
    //TODO Adicionar funcionalidade para múltiplas URI
    this->urls.push_back(url);
    this->url_available = true;
    if(esp_log_level_get(__FILENAME__) == ESP_LOG_INFO){
        printf("\n");
    }
    ESP_LOGI(__FILENAME__, "Added URI: %s\n"
                           "                      Total URIs registered:%d\n", url.c_str(), this->urls.size());

    if(esp_log_level_get(__FILENAME__) == ESP_LOG_DEBUG){
        printf("                      The follow URL are available:\n");
        for(int i=0; i < this->urls.size(); i++) {
            printf("                      %s\n", urls.at(i).c_str());
        }
        printf("\n");
    }
    audio_element_set_uri(get_http_stream_reader(), urls.at(0).c_str());
    return ESP_OK;
}

esp_err_t WebRadio::loop() {
    int player_volume;
    int old_player_volume;
    audio_hal_get_volume(this->board_handle->audio_hal, &player_volume);
    audio_hal_get_volume(this->board_handle->audio_hal, &volume);

    ESP_LOGI(__FILENAME__, "[ 3 ] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), this->evt);

    ESP_LOGI(__FILENAME__, "[3.1] Initialize keys on board");
    audio_board_key_init(set);

    while(this->activated){
        audio_element_state_t el_state = audio_element_get_state(get_i2s_stream_writer());
        if(board_state != el_state){
            switch (board_state) {
                case PLAY:
                    ESP_LOGI(__FILENAME__, "[ * ] Pausing audio pipeline");
                    audio_pipeline_pause(pipeline);
                    break;
                case PAUSED :
                    ESP_LOGI(__FILENAME__, "[ * ] Resuming audio pipeline");
                    audio_pipeline_resume(pipeline);
                    break;
                case STOPPED :
                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_terminate(pipeline);
                    audio_pipeline_reset_ringbuffer(pipeline);
                    break;
                default :
                    ESP_LOGI(__FILENAME__, "[ * ] Not supported state %d", el_state);
            }
        }
        if(volume != old_player_volume){
            audio_hal_set_volume(this->board_handle->audio_hal, volume);
        }
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

        // Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) get_i2s_stream_writer()
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(__FILENAME__, "[ * ] Stop event received");
            break;
        }

        if ((msg.source_type == PERIPH_ID_TOUCH || msg.source_type == PERIPH_ID_BUTTON || msg.source_type == PERIPH_ID_ADC_BTN)
            && (msg.cmd == PERIPH_TOUCH_TAP || msg.cmd == PERIPH_BUTTON_PRESSED || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) {
            if ((int) msg.data == get_input_play_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [Play] touch tap event");
                switch (el_state) {
                    case AEL_STATE_INIT :
                        ESP_LOGI(__FILENAME__, "[ * ] Starting audio pipeline");
                        audio_pipeline_run(pipeline);
                        break;
                    case AEL_STATE_RUNNING :
                        ESP_LOGI(__FILENAME__, "[ * ] Pausing audio pipeline");
                        audio_pipeline_pause(pipeline);
                        break;
                    case AEL_STATE_PAUSED :
                        ESP_LOGI(__FILENAME__, "[ * ] Resuming audio pipeline");
                        audio_pipeline_resume(pipeline);
                        break;
                    case AEL_STATE_FINISHED :
                        ESP_LOGI(__FILENAME__, "[ * ] Rewinding audio pipeline");
                        audio_pipeline_reset_ringbuffer(pipeline);
                        audio_pipeline_reset_elements(pipeline);
                        audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
                        audio_pipeline_run(pipeline);
                        break;
                    default :
                        ESP_LOGI(__FILENAME__, "[ * ] Not supported state %d", el_state);
                }
            } else if ((int) msg.data == get_input_set_id()) {
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
                audio_hal_set_volume(this->board_handle->audio_hal, player_volume);
                ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
            } else if ((int) msg.data == get_input_voldown_id()) {
                ESP_LOGI(__FILENAME__, "[ * ] [Vol-] touch tap event");
                player_volume -= 10;
                if (player_volume < 0) {
                    player_volume = 0;
                }
                audio_hal_set_volume(this->board_handle->audio_hal, player_volume);
                ESP_LOGI(__FILENAME__, "[ * ] Volume set to %d %%", player_volume);
            }
        }
        audio_hal_get_volume(this->board_handle->audio_hal, &old_player_volume);
    }
    return ESP_OK;
}