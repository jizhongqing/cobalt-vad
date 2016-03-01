#ifndef COBALT_GUARD_DETECTOR_H_
#define COBALT_GUARD_DETECTOR_H_

#include "detector/vad_model.h"
#include "util/vad_events.h"

namespace cobalt
{
    // TODO use real audio event from API.
    class AudioEvent;
    class VadDetector
    {
    public:
        VadDetector(VadModel::Ptr model);

        // TODO AudioEvent should be const ref, we need the pointer now for forward declaration.

        // Function for pushing events, returns events as the second argument.
        // Note that events returned here may not be associated with audio pushed,
        // indeed, the events returend could have occurred before or after this chunk of audio, due to buffering
        //
        // Note that the number of events returned is 0-N.
        void pushEvents(AudioEvent *event, VadEvents& events)
    };
}

#endif
