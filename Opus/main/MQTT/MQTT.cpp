//
// Created by Victor Alicino on 25/05/22.
//

#include "MQTT.hpp"

#include <string>

#include "esp_log.h"

const char* MQTT_TAG = __FILENAME__;

extern WebRadio *Radio;

MQTT::instance = nullptr; //Defining Singleton instance as null

MQTT::MQTT() {

}

void MQTT::init(esp_mqtt_client_config_t mqtt_cfg) {
    this->client = esp_mqtt_client_init(&mqtt_cfg);

    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(this->client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID,
                                   this->event_handler, (void *) 0);
    esp_mqtt_client_start(this->client);
}

void MQTT::_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED: mqtt_connected(); break;
        case MQTT_EVENT_DISCONNECTED: mqtt_disconnected(); break;
        case MQTT_EVENT_SUBSCRIBED: mqtt_subscribed(); break;
        case MQTT_EVENT_UNSUBSCRIBED: mqtt_unsubscribed(); break;
        case MQTT_EVENT_PUBLISHED: mqtt_published(); break;
        case MQTT_EVENT_DATA: mqtt_data(event); break;
        case MQTT_EVENT_ERROR: mqtt_error(); break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void MQTT::_mqtt_connected() {
    int msg_id;
    ESP_LOGI(MQTT_TAG, "MQTT Connected");

    msg_id = esp_mqtt_client_publish(client, "board", "LyraT Ativado", 0, 1, 0);
    ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "volume", 0);
    ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, "board", 0);
    ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);
}

void MQTT::_mqtt_disconnected() {
    ESP_LOGI(MQTT_TAG, "MQTT Disconnected");
}

void MQTT::_mqtt_subscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Subscribed");
}

void MQTT::_mqtt_unsubscribed() {
    ESP_LOGI(MQTT_TAG, "MQTT Unsubscribed");
}

void MQTT::_mqtt_published() {
    ESP_LOGI(MQTT_TAG, "MQTT Published");
}

void MQTT::_mqtt_data(esp_mqtt_event_handle_t event) {
    ESP_LOGI(MQTT_TAG, "MQTT Data");

    if(event->topic == "volume"){
        Radio->change_volume_to(std::atoi(payload_s.c_str()));
    }
    if(event->topic == "board"){
        if(event->data == "play") {
            ESP_LOGI(MQTT_TAG, "PLAY");
            Radio->resume();
        }
        if(event->data == "pause") {
            Radio->pause();
            ESP_LOGI(MQTT_TAG, "PAUSE");
        }
        if(event->data == "stop") {
            Radio->stop();
            ESP_LOGI(MQTT_TAG, "STOP");
        }
        if(event->data == "start") {
            Radio->restart();
            Radio->run();
            ESP_LOGI(MQTT_TAG, "Pipeline started");
        }
    }
}

void MQTT::_mqtt_error() {
    ESP_LOGE(MQTT_TAG, "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

    }
}
