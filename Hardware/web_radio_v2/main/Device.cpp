//
// Created by Victor Alicino on 26/05/22.
//

#include "Device.hpp"

#include <string>

Device::Device() {

}

std::string Device::get_full_name() {
    std::string full_name;
    full_name = this->name + " (" + 
                this->code + ")";
    return full_name;
}

std::string Device::get_name() {
    return this->name;
}

std::string Device::get_code() {
    return this->code;
}