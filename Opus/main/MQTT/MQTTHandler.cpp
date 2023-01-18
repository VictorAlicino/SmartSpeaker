//
// Created by Victor Alicino on 17/10/22.
//

#include "MQTTHandler.h"
#include "MQTT/MQTT.hpp"
#include "mqtt_client.h"
#include "esp_log.h"
#include <cstring>

//TODO: Adjust the LOG prints

const char* MQTT_TAG = __FILENAME__;

void _event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    static MQTTHandler handler;
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED: handler._mqtt_connected(); break;
        case MQTT_EVENT_DISCONNECTED: handler._mqtt_disconnected(); break;
        case MQTT_EVENT_SUBSCRIBED: handler._mqtt_subscribed(); break;
        case MQTT_EVENT_UNSUBSCRIBED: handler._mqtt_unsubscribed(); break;
        case MQTT_EVENT_PUBLISHED: handler._mqtt_published(); break;
        case MQTT_EVENT_DATA: handler._mqtt_data(event); break;
        case MQTT_EVENT_ERROR: handler._mqtt_error(event); break;
        default:
            ESP_LOGI(MQTT_TAG, "MQTT -> Other event id:%d", event->event_id);
            break;
    }
}

void MQTTHandler::_mqtt_connected() {
    int msg_id;
    ESP_LOGI(MQTT_TAG, "MQTT Connected");

    esp_mqtt_client_handle_t client = MQTT::get_instance()->client;

    msg_id = MQTTHandler::publish("board", "LyraT Ativado", 0, 1, 0);
    ESP_LOGI(MQTT_TAG, "MQTT -> sent publish successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "volume", 0);
    ESP_LOGI(MQTT_TAG, "MQTT -> sent subscribe successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "board", 0);
    ESP_LOGI(MQTT_TAG, "MQTT -> sent subscribe successful, msg_id=%d", msg_id);
}

void MQTTHandler::_mqtt_disconnected() {
    ESP_LOGI(MQTT_TAG, "MQTT Disconnected");
}

void MQTTHandler::_mqtt_subscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Subscribed");
}

void MQTTHandler::_mqtt_unsubscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Unsubscribed");
}

void MQTTHandler::_mqtt_published() {
    ESP_LOGI(MQTT_TAG, "MQTT Published");
}

void MQTTHandler::_mqtt_data(esp_mqtt_event_handle_t event) {
    ESP_LOGI(MQTT_TAG, "MQTT Data");

    //std::string_view topic(event->topic, event->topic_len);
    //std::string_view data(event->data, event->data_len);

    if(strncmp(event->topic, "volume", event->topic_len) == 0){
        //TODO: Change volume
    }
    if(strncmp(event->topic, "board", event->topic_len) == 0){
        if(strncmp(event->data, "desligar", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "MQTT -> PLAY");
        }
        if(strncmp(event->data, "ligar", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "MQTT -> PAUSE");
        }
        if(strncmp(event->data, "play", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "MQTT -> STOP");
        }
        if(strncmp(event->data, "pause", event->data_len) == 0){
            ESP_LOGI(MQTT_TAG, "MQTT -> Pipeline started");
        }
    }
}

void MQTTHandler::_mqtt_error(esp_mqtt_event_handle_t event) {
    ESP_LOGE(MQTT_TAG, "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        //TODO: Treat MQTT error
    }
}

int MQTTHandler::publish(const char *topic, const char *data, int len, int qos, int retain) {
    esp_mqtt_client_handle_t client = MQTT::get_instance()->client;
    int msg_id = esp_mqtt_client_publish(client, topic, data, len, qos, retain);
    ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);
    return msg_id;
}

int MQTTHandler::subscribe(const char *topic, int qos) {
    esp_mqtt_client_handle_t client = MQTT::get_instance()->client;
    int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
    ESP_LOGI(MQTT_TAG, "OPUS is subscribed to topic %s; MSG_ID=%d", topic, msg_id);
    return msg_id;
}