//
// Created by Victor Alicino on 26/05/22.
//

#ifndef SMART_SPEAKER_DEVICE_H
#define SMART_SPEAKER_DEVICE_H

#include <string>

class Device{
private:
    std::string code;
    std::string name;

public:
    Device();
    std::string get_full_name;
    std::string get_name;
    std::string get_code;
}

#endif //SMART_SPEAKER_DEVICE_H