//
// Created by Victor Alicino on 19/10/22.
//

#ifndef OPUS_AUDIOPIPELINE_H
#define OPUS_AUDIOPIPELINE_H

#include <vector>
#include <string>
#include "audio_element.h"
#include "audio_event_iface.h"
#include "audio_pipeline.h"

/**
 * @brief Audio pipeline state
 */
typedef enum {
    AP_STATE_RUNNING,
    AP_STATE_STOPPED,
    AP_STATE_PAUSED,
    AP_STATE_ERROR,
} audio_pipeline_state_t;

class AudioPipeline {
private:
    //Pipeline
    audio_pipeline_state_t pipeline_state;
    audio_pipeline_handle_t pipeline;

    //Event handlers
    audio_event_iface_handle_t evt;

    //Singleton
    AudioPipeline();

public:
    //Get instance
    static AudioPipeline &getInstance();

    /**
     * @brief Register an element to the pipeline
     * @param element Element to register
     */
    void register_element(audio_element_handle_t element);
};


#endif //OPUS_AUDIOPIPELINE_H
