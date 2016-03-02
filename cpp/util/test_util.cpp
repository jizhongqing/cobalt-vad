#include "util/vad_events.h"

using namespace cobalt;
using namespace std;

string makeAndPrintJson(const VadEvents& events)
{
    JsonValuePtr jsonEvents = serializeVadEvents(events);
    Json::FastWriter fastWriter;
    fastWriter.omitEndingLineFeed();
    string serializedEvents = fastWriter.write(*jsonEvents);
    cout << "Serialized vad events " << endl << serializedEvents << endl;
    return serializedEvents;
}

VadEvent makeVadEvent(VadEventType type, int eventOccurenceMsec)
{
    VadEvent event;
    event.vadEventType = type;
    event.eventOccurenceMsec = eventOccurenceMsec;
    return event;
}

void printVadEvents(const VadEvents& events)
{
    cout << "Printing " << events.size() << " VAD events." << endl;
    for (size_t i = 0; i < events.size(); ++i)
    {
        const string eventType = (events[i].vadEventType == SOS) ? "start of speech" : "end of speech";
        cout << "event " << i << " type: " << eventType << " event occurence msec: " << events[i].eventOccurenceMsec << endl;
    }
}

VadEvents deserizalizeAndPrint(const string& serializedEvents)
{
    VadEvents events;
    deserializeVadEvents(serializedEvents, events);
    printVadEvents(events);
    return events;
}

bool compareEvent(const VadEvent& eventA, const VadEvent& eventB)
{
    return (eventA.eventOccurenceMsec==eventB.eventOccurenceMsec &&
            eventA.vadEventType == eventB.vadEventType);
}

bool compareEvents(const VadEvents& eventsA, const VadEvents& eventsB)
{
    if (eventsA.size() != eventsB.size())
    {
        return false;
    }
    for (size_t i = 0; i < eventsA.size(); ++i)
    {
        if (!compareEvent(eventsA[i], eventsB[i]))
        {
            return false;
        }
    }
    return true;
}

void test_vad_events()
{
    cout << endl << endl << "Begin test_vad_events()" << endl;

    VadEvents events;
    events.push_back(makeVadEvent(SOS, 10));
    events.push_back(makeVadEvent(EOS, 200));
    events.push_back(makeVadEvent(SOS, 210));
    events.push_back(makeVadEvent(EOS, 300));
    string serializedEvents = makeAndPrintJson(events);

    cout << endl << "de-serialized vad events:" << endl;
    VadEvents deserialziedEvents = deserizalizeAndPrint(serializedEvents);
    if(!compareEvents(events, deserialziedEvents))
    {
        throw std::runtime_error("serialized and deserialized events not equal");
    }
    cout << endl << "test no vad events" << endl;
    VadEvents emptyEvents;
    string serializedEmptyEvents = makeAndPrintJson(emptyEvents);
    cout << endl << "de-serialized empty vad events:" << endl;
    VadEvents deserializedEmptyEvents;deserizalizeAndPrint(serializedEmptyEvents);

    if(!compareEvents(emptyEvents, deserializedEmptyEvents))
    {
        throw std::runtime_error("serialized and deserialized events not equal");
    }
    cout << endl << endl << "End test_vad_events()" << endl;
}

void testVadEventsFromFrames()
{
    cout << endl << endl << "Begin testVadEventsFromFrames()" << endl;
    // first make some events.
    kaldi::Vector<float> frameVadInfo;
    frameVadInfo.Resize(30);
    for (int i = 0 ; i < 10; ++i)
    {
        frameVadInfo(i) = 1;
    }
    for (int i = 10 ; i < 20; ++i)
    {
        frameVadInfo(i) = 0;
    }
    for (int i = 20 ; i < 30; ++i)
    {
        frameVadInfo(i) = 1;
    }
    VadEvents events;
    int previousEndFrame = 0;
    bool previousState = false;
    frameVadResultsToEvents(frameVadInfo, events, previousEndFrame, previousState);
    cout << endl << "events with previousEndFrame " << previousEndFrame << " previousState " << previousState << endl;
    printVadEvents(events);

    previousState = true;
    frameVadResultsToEvents(frameVadInfo, events, previousEndFrame, previousState);
    cout << "events with previousEndFrame " << previousEndFrame << " previousState " << previousState << endl;
    printVadEvents(events);

    previousEndFrame = 10;
    previousState = false;
    frameVadResultsToEvents(frameVadInfo, events, previousEndFrame, previousState);
    cout << endl << "events with previousEndFrame " << previousEndFrame << " previousState " << previousState << endl;
    printVadEvents(events);

    frameVadInfo.Resize(0);
    cout << endl << "Try converting to events with 0 frames" << endl;
    frameVadResultsToEvents(frameVadInfo, events, previousEndFrame, previousState);
    printVadEvents(events);

    cout << endl << endl << "End testVadEventsFromFrames()" << endl;
}

int main()
{
    try
    {
        test_vad_events();
        testVadEventsFromFrames();
    }
    catch (std::runtime_error &e)
    {
        cout << "Caught C++ runtime error: " << e.what() << endl;
        return 1;
    }
    catch ( ... )
    {
        cout << "Caught unknown error!!";
        return 2;
    }
    return 0;
}
