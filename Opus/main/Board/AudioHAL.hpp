//
// Created by victo on 27/01/2023.
//

#ifndef SMARTSPEAKER_AUDIOHAL_HPP
#define SMARTSPEAKER_AUDIOHAL_HPP

#include "audio_element.h"


class AudioHAL {
private:
    //Singleton instance
    static AudioHAL* instance;

    audio_board_handle_t board_handle; // Audio board handle

    AudioHAL();

public:

    /**
     * @brief Get singleton instance
     * @return Singleton instance
     */
    static AudioHAL* get_instance();

    esp_err_t init();

};


#endif //SMARTSPEAKER_AUDIOHAL_HPP
