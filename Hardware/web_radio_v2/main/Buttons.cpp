//
// Created by Victor Alicino on 23/05/22.
//

#include "Buttons.h"

#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"


Buttons::Buttons() {
    audio_board_key_init(set);
}

void Buttons::loop() {
    audio_event_iface_msg_t msg;
    esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
    if (ret != ESP_OK) {
        continue;
    }
    if ((msg.source_type == PERIPH_ID_TOUCH || msg.source_type == PERIPH_ID_BUTTON || msg.source_type == PERIPH_ID_ADC_BTN)
        && (msg.cmd == PERIPH_TOUCH_TAP || msg.cmd == PERIPH_BUTTON_PRESSED || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) {
        if ((int) msg.data == get_input_play_id()) {
            ESP_LOGI(TAG, "[ * ] [Play] touch tap event");
            audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
            switch (el_state) {
                case AEL_STATE_INIT :
                    ESP_LOGI(TAG, "[ * ] Starting audio pipeline");
                    audio_pipeline_run(pipeline);
                    break;
                case AEL_STATE_RUNNING :
                    ESP_LOGI(TAG, "[ * ] Pausing audio pipeline");
                    audio_pipeline_pause(pipeline);
                    break;
                case AEL_STATE_PAUSED :
                    ESP_LOGI(TAG, "[ * ] Resuming audio pipeline");
                    audio_pipeline_resume(pipeline);
                    break;
                case AEL_STATE_FINISHED :
                    ESP_LOGI(TAG, "[ * ] Rewinding audio pipeline");
                    audio_pipeline_reset_ringbuffer(pipeline);
                    audio_pipeline_reset_elements(pipeline);
                    audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
                    set_next_file_marker();
                    audio_pipeline_run(pipeline);
                    break;
                default :
                    ESP_LOGI(TAG, "[ * ] Not supported state %d", el_state);
            }
        } else if ((int) msg.data == get_input_set_id()) {
            ESP_LOGI(TAG, "[ * ] [Set] touch tap event");
            ESP_LOGI(TAG, "[ * ] Stopping audio pipeline");
            break;
        } else if ((int) msg.data == get_input_mode_id()) {
            ESP_LOGI(TAG, "[ * ] [mode] tap event");
            audio_pipeline_stop(pipeline);
            audio_pipeline_wait_for_stop(pipeline);
            audio_pipeline_terminate(pipeline);
            audio_pipeline_reset_ringbuffer(pipeline);
            audio_pipeline_reset_elements(pipeline);
            set_next_file_marker();
            audio_pipeline_run(pipeline);
        } else if ((int) msg.data == get_input_volup_id()) {
            ESP_LOGI(TAG, "[ * ] [Vol+] touch tap event");
            player_volume += 10;
            if (player_volume > 100) {
                player_volume = 100;
            }
            audio_hal_set_volume(board_handle->audio_hal, player_volume);
            ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
        } else if ((int) msg.data == get_input_voldown_id()) {
            ESP_LOGI(TAG, "[ * ] [Vol-] touch tap event");
            player_volume -= 10;
            if (player_volume < 0) {
                player_volume = 0;
            }
            audio_hal_set_volume(board_handle->audio_hal, player_volume);
            ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
        }
    }

}
