//
// Created by Victor Alicino on 17/10/22.
//

#ifndef OPUS_MQTTHANDLER_H
#define OPUS_MQTTHANDLER_H

#include "mqtt_client.h"
#include "MQTT/MQTT.hpp"

//TODO: Finish the documentation
/**
 * @brief Handler main function
 * @param handler_args
 * @param base
 * @param event_id
 * @param event_data
 */
void _event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief This class is a wrapper for the ESP-MQTT library, for more details in its usage please refer to the ESP-MQTT
 * library documentation on:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/mqtt.html
 */
class MQTTHandler {
public:
    /**
     * @brief   MQTT Callback, called when the client has successfully established a connection to the broker.
     *          The client is now ready to send and receive data.
     */
    void _mqtt_connected();

    /**
     * @brief   MQTT Callback, called when the client has aborted the connection due to being unable to read or write
     *          data, e.g. because the server is unavailable.
     */
    void _mqtt_disconnected();

    /**
     * @brief   MQTT Callback, called when the broker has acknowledged the client’s subscribe request.
     *          The event data will contain the message ID of the subscribe message.
     */
    void _mqtt_subscribed();

    /**
     * @brief   MQTT Callback, called when the broker has acknowledged the client’s unsubscribe request.
     *          The event data will contain the message ID of the unsubscribe message.
     */
    void _mqtt_unsubscribed();

    /**
     * @brief   MQTT Callback, called when the broker has acknowledged the client’s publish message.
     *          This will only be posted for Quality of Service level 1 and 2, as level 0 does not use acknowledgements.
     *          The event data will contain the message ID of the publish message.
     */
    void _mqtt_published();

    /**
     * @brief   MQTT Callback, called when the client has received a publish message.
     *          The event data contains: message ID, name of the topic it was published to,
     *          received data and its length. For data that exceeds the internal buffer multiple MQTT_EVENT_DATA will
     *          be posted and current_data_offset and total_data_len from event data updated to keep track of the
     *          fragmented message.
     * @param event MQTT event
     */
    void _mqtt_data(esp_mqtt_event_handle_t event);

    /**
     * @brief   MQTT Callback, called when the client has encountered an error.
     */
    void _mqtt_error(esp_mqtt_event_handle_t event);

    /**
     * @brief           Publish data to topic
     * @param topic     Topic to publish to
     * @param data      Data to publish
     * @param len       Length of data
     * @param qos       Quality of service level
     * @param retain    Retain flag
     * @return          message_id of the publish message (for QoS 0 message_id will always be zero) on success.
     *                  -1 on failure.
     */
    static int publish(const char *topic, const char *data, int len, int qos, int retain);

    /**
     * @brief       Subscribe to topic
     * @param topic Topic to subscribe to
     * @param qos   Quality of service level
     * @return      message_id of the subscribe message on success. -1 on failure.
     */
    static int subscribe(const char *topic, int qos);
};


#endif //OPUS_MQTTHANDLER_H
