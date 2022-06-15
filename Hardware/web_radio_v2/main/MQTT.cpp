//
// Created by Victor Alicino on 25/05/22.
//

#include "MQTT.hpp"

#include <string>

#include "esp_log.h"
#include "mqtt_client.h"
#include "Audio/WebRadio.hpp"

const char* MQTT_TAG = __FILENAME__;

extern WebRadio *Radio;

MQTT::MQTT(std::string server_uri, WebRadio web_radio) {
    const char* server_uri_c = server_uri.c_str();
    esp_mqtt_client_config_t mqtt_cfg = {
            .uri = server_uri_c,
    };

    this->board_handle = web_radio.get_board_handle();
    this->client = esp_mqtt_client_init(&mqtt_cfg);

    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(this->client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, this->event_handler, nullptr);
    esp_mqtt_client_start(this->client);

}

void MQTT::event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    char payload_s[50];
    char topic_s[50];
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "board", "LyraT Ativado", 0, 1, 0);
            ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "volume", 0);
            ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "board", 0);
            ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
            strncpy(payload_s, event->data, event->data_len);
            strncpy(topic_s, event->topic, event->topic_len);
            payload_s[event->data_len] = 0;
            topic_s[event->topic_len] = 0;

            if(strcmp(topic_s, "volume") == 0){
                Radio->change_volume_to(std::atoi(payload_s));
            }
            if(strcmp(topic_s, "board") == 0){
                if(strcmp(payload_s, "play") == 0) {
                    ESP_LOGI(MQTT_TAG, "PLAY");
                    Radio->resume();
                }
                if(strcmp(payload_s, "pause") == 0) {
                    Radio->pause();
                    ESP_LOGI(MQTT_TAG, "PAUSE");
                }
                if(strcmp(payload_s, "stop") == 0) {
                    Radio->stop();
                    ESP_LOGI(MQTT_TAG, "STOP");
                }
                if(strcmp(payload_s, "start") == 0) {
                    Radio->restart();
                    Radio->run();
                    ESP_LOGI(MQTT_TAG, "Pipeline started");
                }
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(MQTT_TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

            }
            break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
            break;
    }
}