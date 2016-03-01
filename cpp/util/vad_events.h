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

    JsonValuePtr serializeVadEvents(const VadEvents& events);

    void deserializeVadEvents(const std::string& events, VadEvents& deserializedEvents);
}

#endif
