#ifndef COBALT_GUARD_VAD_EVENTS_H_
#define COBALT_GUARD_VAD_EVENTS_H_

#include <string>
#include <vector>
#include "util/io.h"

namespace cobalt
{
    typedef enum
    {
        SOS, // start of speech
        EOS // end of speech
    } VadEventType;

    struct VadEvent
    {
        VadEventType vadEventType;
        int eventOccurenceMsec;
    };

    typedef std::vector<VadEvent> VadEvents;

    // function to turn per frame based VAD events into just vad events.
    // previousEndFrame allows us to keep track of where we ended on the previous frame.
    // previousState allows us to keep track of what state we ended on a previous call.
    void frameVadResultsToEvents(const std::vector<bool> &vadResults, VadEvents& events, int previousEndFrame, bool previousState);

    JsonValuePtr serializeVadEvents(const VadEvents& events);

    void deserializeVadEvents(const std::string& events, VadEvents& deserializedEvents);

    bool kaldiVadToBool(float kaldiVad);
}

#endif
