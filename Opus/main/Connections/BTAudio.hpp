//
// Created by victo on 17/01/2023.
//

#ifndef SMARTSPEAKER_BTAUDIO_H
#define SMARTSPEAKER_BTAUDIO_H


class BTAudio {
private:
    BTAudio();
    static BTAudio* instance;

public:
    static BTAudio* get_instance();


};


#endif //SMARTSPEAKER_BTAUDIO_H
