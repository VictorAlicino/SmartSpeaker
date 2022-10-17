//
// Created by Victor Alicino on 17/10/22.
//

#include "MQTTRadio.h"
#include "MQTT/MQTT.hpp"
#include "mqtt_client.h"
#include "esp_log.h"
#include <cstring>

const char* MQTT_TAG = __FILENAME__;

void radio_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    static MQTTRadio radio;
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED: radio._mqtt_connected(); break;
        case MQTT_EVENT_DISCONNECTED: radio._mqtt_disconnected(); break;
        case MQTT_EVENT_SUBSCRIBED: radio._mqtt_subscribed(); break;
        case MQTT_EVENT_UNSUBSCRIBED: radio._mqtt_unsubscribed(); break;
        case MQTT_EVENT_PUBLISHED: radio._mqtt_published(); break;
        case MQTT_EVENT_DATA: radio._mqtt_data(event); break;
        case MQTT_EVENT_ERROR: radio._mqtt_error(event); break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void MQTTRadio::_mqtt_connected() {
    int msg_id;
    ESP_LOGI(MQTT_TAG, "MQTT Connected");

    esp_mqtt_client_handle_t client = MQTT::getInstance()->client;

    msg_id = esp_mqtt_client_publish(client, "board", "LyraT Ativado", 0, 1, 0);
    ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "volume", 0);
    ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "board", 0);
    ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);
}

void MQTTRadio::_mqtt_disconnected() {
    ESP_LOGI(MQTT_TAG, "MQTT Disconnected");
}

void MQTTRadio::_mqtt_subscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Subscribed");
}

void MQTTRadio::_mqtt_unsubscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Unsubscribed");
}

void MQTTRadio::_mqtt_published() {
    ESP_LOGI(MQTT_TAG, "MQTT Published");
}

void MQTTRadio::_mqtt_data(esp_mqtt_event_handle_t event) {
    ESP_LOGI(MQTT_TAG, "MQTT Data");

    //std::string_view topic(event->topic, event->topic_len);
    //std::string_view data(event->data, event->data_len);

    if(strncmp(event->topic, "volume", event->topic_len) == 0){
        //TODO: Change volume
    }
    if(strncmp(event->topic, "board", event->topic_len) == 0){
        if(strncmp(event->data, "desligar", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "PLAY");
        }
        if(strncmp(event->data, "ligar", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "PAUSE");
        }
        if(strncmp(event->data, "play", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "STOP");
        }
        if(strncmp(event->data, "pause", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "Pipeline started");
        }
    }
}

void MQTTRadio::_mqtt_error(esp_mqtt_event_handle_t event) {
    ESP_LOGE(MQTT_TAG, "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        //TODO: Treat MQTT error
    }
}