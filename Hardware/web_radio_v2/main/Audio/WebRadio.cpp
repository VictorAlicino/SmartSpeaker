//
// Created by Victor Alicino on 23/05/22.
//

#include "WebRadio.hpp"
#include "MQTT.hpp"
#include "Device.hpp"

#include "esp_log.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "i2s_stream.h"
#include "audio_pipeline.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "board.h"

#include "math.h"

extern Device* Board;

const char* WEBRADIO_TAG = __FILENAME__;

WebRadio::WebRadio() {
    this->volume_old = 50;
    this->url_available = false;
}

esp_err_t WebRadio::add_uri(std::string url) {
    //TODO Adicionar funcionalidade para múltiplas URI
    this->urls.push_back(url);
    this->url_available = true;
    if(esp_log_level_get(WEBRADIO_TAG) == ESP_LOG_INFO){
        printf("\n");
    }
    ESP_LOGI(WEBRADIO_TAG, "Added URI: %s\n"
                           "                      Total URIs registered:%d\n", url.c_str(), this->urls.size());

    if(esp_log_level_get(WEBRADIO_TAG) == ESP_LOG_DEBUG){
        printf("                      The follow URL are available:\n");
        for(int i=0; i < this->urls.size(); i++) {
            printf("                      %s\n", urls.at(i).c_str());
        }
        printf("\n");
    }
    audio_element_set_uri(this->http_stream_reader, urls.at(0).c_str());
    return ESP_OK;
}

esp_err_t WebRadio::link_to_pipeline(AUDIO_STREAM reader, AUDIO_CODEC codec, AUDIO_STREAM writer) {
    esp_err_t er = ESP_OK;

    // Setting up Reader
    if(reader == HTTP_STREAM) {
        audio_pipeline_register(this->pipeline, this->http_stream_reader, "http");
        this->link_tag.push_back("http");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "http");
    } else er = ESP_FAIL;

    // Setting up CODEC
    if(codec == OGG_DECODER) {
        audio_pipeline_register(this->pipeline, this->ogg_decoder, "ogg");
        this->link_tag.push_back("ogg");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "ogg");
    } else er = ESP_FAIL;

    // Setting up Writer
    if(writer == I2S_STREAM) {
        audio_pipeline_register(this->pipeline, this->i2s_stream_writer, "i2s");
        this->link_tag.push_back("i2s");
        ESP_LOGD(WEBRADIO_TAG, "Element <<%s>> registered to Pipeline", "i2s");
    } else er = ESP_FAIL;

    // Linking all to PIPELINE
    const char* r = link_tag.at(0).c_str();
    const char* c = link_tag.at(1).c_str();
    const char* w = link_tag.at(2).c_str();
    const char* link_tag_t[3] = {r, c, w};
    er = audio_pipeline_link(this->pipeline, &link_tag_t[0], link_tag.size());

    if (er == ESP_OK) ESP_LOGD(WEBRADIO_TAG, "Elements linked to pipeline");

    return er;
}

esp_err_t WebRadio::change_volume_to(int volume){
    esp_err_t err = ESP_OK;
    int cur_volume;
    const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
    audio_hal_get_volume(this->board_handle->audio_hal, &cur_volume);
    ESP_LOGI(WEBRADIO_TAG, "Changing Volume from %d to %d", cur_volume, volume);

    // Handling out of range volumes
    if(volume > 100) audio_hal_set_volume(this->board_handle->audio_hal, 100);
    if(volume < 0) audio_hal_set_volume(this->board_handle->audio_hal, 0);

    // Audio volume transition
    if(cur_volume < volume){
        for(;cur_volume < volume; cur_volume++) {
            audio_hal_set_volume(this->board_handle->audio_hal, cur_volume);
            vTaskDelay(xDelay*200);
        }
    }else{
        for(;cur_volume > volume; cur_volume--) {
            audio_hal_set_volume(this->board_handle->audio_hal, cur_volume);
            vTaskDelay(xDelay*350);
        }
    }
    // End of Audio volume transition

    return err;
}

esp_err_t WebRadio::loop() {
    int player_volume;

    audio_hal_get_volume(
            this->board_handle->audio_hal,
            &player_volume
            );

    esp_periph_set_handle_t set = Board->peripheral_init(
            "set1",
            DEFAULT_ESP_PERIPH_SET_CONFIG()
            );

    audio_event_iface_set_listener(
            esp_periph_set_get_event_iface(set),
            this->evt
            );

    ESP_LOGI(WEBRADIO_TAG, "Initializing keys on board");
    audio_board_key_init(set);

    while(this->pipeline_state == ACTIVE){
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(this->evt, &msg, portMAX_DELAY);

        if (ret != ESP_OK) {
            ESP_LOGE(WEBRADIO_TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        // If event receive an info data
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
        && msg.source == (void *) this->ogg_decoder
        && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
                on_music_info();
                continue;
        }

        // Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
        && msg.source == (void *) this->i2s_stream_writer
        && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
        && (((int)msg.data == AEL_STATUS_STATE_STOPPED)
        || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(WEBRADIO_TAG, "Stop event received");
            this->pipeline_state = STOPPED;
        }

        if (Board->is_board_button_event(msg)){
            if(msg.cmd == 0){
                // Button status unchanged
                ESP_LOGI(WEBRADIO_TAG, "Button status unchanged");
            }
            if((int) msg.data == Board->button.play){
                // Play button pressed
                on_play_button_pressed();
            }
            else if((int) msg.data == Board->button.set){
                // Set button pressed
                //TODO Mudar URI da rádio web
            }
            else if((int) msg.data == Board->button.volup){
                // Vol+ button pressed
                change_volume_to(player_volume + 10);
            }
            else if((int) msg.data == Board->button.voldown){
                // Vol- button pressed
                change_volume_to(player_volume - 10);

            }
            else if((int) msg.data == Board->button.mode){
                // Mode button pressed
            }
            else if((int) msg.data == Board->button.rec){
                // Rec button pressed
            }
        }
        audio_hal_get_volume(this->board_handle->audio_hal, &player_volume);
    }
    return ESP_OK;
}

void WebRadio::on_music_info() {
    audio_element_info_t music_info = {
            0
    };
    audio_element_getinfo(
            this->ogg_decoder,
            &music_info
    );

    ESP_LOGI(WEBRADIO_TAG, "Receive music info from ogg decoder\nsample_rates=%d\nbits=%d\nch=%d",
             music_info.sample_rates, music_info.bits, music_info.channels);

    audio_element_setinfo(this->i2s_stream_writer, &music_info);
    i2s_stream_set_clk(this->i2s_stream_writer, music_info.sample_rates,
                       music_info.bits, music_info.channels);
}

void WebRadio::on_play_button_pressed() {
    audio_element_state_t el_state = audio_element_get_state(this->i2s_stream_writer);

    if(el_state == AEL_STATE_INIT){
        run();
    }
    else if(el_state == AEL_STATE_RUNNING){
        pause();
    }
    else if(el_state == AEL_STATE_PAUSED){
        resume();
    }
    else if(el_state == AEL_STATE_FINISHED){
        restart();
    }
    else{
        ESP_LOGI(WEBRADIO_TAG, "Not supported state %d", el_state);
    }
}