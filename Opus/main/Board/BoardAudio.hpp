//
// Created by victo on 27/01/2023.
//

#ifndef SMARTSPEAKER_BOARDAUDIO_HPP
#define SMARTSPEAKER_BOARDAUDIO_HPP

#include "board.h"


class BoardAudio {
private:
    //Singleton instance
    static BoardAudio* instance;

    audio_board_handle_t board_handle; // Audio board handle

    BoardAudio();

public:

    /**
     * @brief Get singleton instance
     * @return Singleton instance
     */
    static BoardAudio* get_instance();

    esp_err_t init();

};


#endif //SMARTSPEAKER_BOARDAUDIO_HPP
