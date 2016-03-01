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

void deserizalizeAndPrint(const string& serializedEvents)
{
    VadEvents events;
    deserializeVadEvents(serializedEvents, events);
    printVadEvents(events);
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
    deserizalizeAndPrint(serializedEvents);

    cout << endl << "test no vad events" << endl;
    VadEvents emptyEvents;
    string serializedEmptyEvents = makeAndPrintJson(emptyEvents);
    cout << endl << "de-serialized empty vad events:" << endl;
    deserizalizeAndPrint(serializedEmptyEvents);
    cout << endl << endl << "End test_vad_events()" << endl;
}

int main()
{
    try
    {
        test_vad_events();
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
