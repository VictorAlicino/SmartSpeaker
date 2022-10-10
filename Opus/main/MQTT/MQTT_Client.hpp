//
// Created by Victor Alicino on 10/10/22.
//

#ifndef OPUS_MQTT_CLIENT_H
#define OPUS_MQTT_CLIENT_H

// Create a new Singleton MQTT Client
class MQTT_Client {
    private:
        MQTT_Client();
        static MQTT_Client* instance;
public:
    static MQTT_Client &getInstance();
    void init();
    void de_init();
};

#endif //OPUS_MQTT_CLIENT_H
