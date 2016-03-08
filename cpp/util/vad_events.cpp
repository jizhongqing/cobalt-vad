#include "util/vad_events.h"

namespace cobalt
{
    namespace vad_events_json
    {
        const string VAD_EVENTS = "vad_events";
        const string VERSION = "1";
        const string EVENTS = "events";
        const string START_OF_SPEECH = "start_of_speech";
        const string END_OF_SPEECH = "end_of_speech";
        const string EVENT_TYPE = "event_type";
        const string EVENT_OCCURENCE_MSEC = "event_occurence_msec";
    }

    JsonValuePtr serializeVadEvents(const VadEvents& events)
    {
        using namespace vad_events_json;
        JsonValuePtr root = makeCobaltJson(VAD_EVENTS, VERSION);
        Json::Value vadEvents;

        for (size_t i = 0; i < events.size(); ++i)
        {
            Json::Value vadEvent;
            vadEvent[EVENT_TYPE] = (events[i].vadEventType == SOS) ? START_OF_SPEECH : END_OF_SPEECH;
            vadEvent[EVENT_OCCURENCE_MSEC] = events[i].eventOccurenceMsec;
            vadEvents.append(vadEvent);
        }
        (*root)[EVENTS] = vadEvents;
        return root;
    }

    // true is in-speech, false is silence
    bool kaldiVadToBool(float kaldiVad)
    {
        if (kaldiVad == 1.0)
        {
            return true;
        }
        else if (kaldiVad == 0.0)
        {
            return false;
        }
        throw std::runtime_error("kaldi vad was neither 0 or 1.");
    }

    void frameVadResultsToEvents(const std::vector<bool> &vadResults, VadEvents& events, int previousEndFrame, bool previousState)
    {
        events.clear();
        // this value is constant for all kaldi frames.
        const int millisecondsPerFrame = 10;
        for (size_t i = 0; i < vadResults.size(); ++i)
        {
            const bool currentState = vadResults[i];
            // current state != previous state, an event occurred.
            if (currentState != previousState)
            {
                VadEvent event;
                const int currentFrame = previousEndFrame + i;
                event.eventOccurenceMsec = currentFrame * millisecondsPerFrame;
                // we went from silence to speech, this is a start-of-speech event.
                if (currentState)
                {
                    event.vadEventType = SOS;
                }
                // we went from speech to silence, this is a end-of-speech event
                else
                {
                    event.vadEventType = EOS;
                }
                events.push_back(event);
                previousState = currentState;
            }
        }

    }

    void deserializeVadEvents(const string& jsonEvents, VadEvents& deserializedEvents)
    {
        using namespace vad_events_json;
        deserializedEvents.clear();
        Json::Value root;   // will contains the root value after parsing.
        Json::Reader reader;
        if (!reader.parse(jsonEvents, root))
        {
            throw std::runtime_error("Could not parse json vad events " + jsonEvents);
        }

        Json::Value empty;
        if (!root.isMember(EVENTS))
        {
            throw std::runtime_error("Expected member " + EVENTS + " not found in JSON " + jsonEvents);
        }
        Json::Value events = root.get(EVENTS, empty);
        for (int i = 0; i < events.size(); ++i)
        {
            VadEvent event;
            event.eventOccurenceMsec = events[i].get(EVENT_OCCURENCE_MSEC, empty).asInt();
            event.vadEventType = (events[i].get(EVENT_TYPE, empty).asString() == START_OF_SPEECH) ? SOS : EOS;
            deserializedEvents.push_back(event);
        }
    }
}
