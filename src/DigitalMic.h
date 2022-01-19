//
// Created by Victor Alicino on 19/01/22.
//

#ifndef SMARTSPEAKER_DIGITALMIC_H
#define SMARTSPEAKER_DIGITALMIC_H
#include <Arduino.h>
#include "driver/i2s.h"
#include "freertos/queue.h"

class DigitalMic{
private:
    static const int i2s_num = 0; // i2s port number
public:
    DigitalMic(int mck, int bck, int ws, int data_out, int data_in);
};


#endif //SMARTSPEAKER_DIGITALMIC_H
