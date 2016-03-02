#include "client/client_utils.h"
#include "detector/audio_io.h"
#include "api/api.h"

const string gRealModelId = "realModel";
bool gDolog = true;
boost::mutex gMutex;
AudioEvent gEvent;
using namespace cobalt;
using namespace vad_client;
using namespace std;

void testLoggingCallback(int loglevel, CobaltString msg)
{
    if(gDolog)
    {
        // mutex inside conditional for efficiency.
        boost::lock_guard<boost::mutex> lock(gMutex);
        cout << "TestLogger: level " << loglevel << " message " << msg << endl;
    }
}


// Steady-state recognition. We've already setup callbacks and a model, we are ready
// to recognize speech.
//
// This function
// 1. Instantiate a recognizer.
// 2. Pushes an audio event to the recognizer.
// 3. Check result of audio event, and call GetResult() if there is no result.
// 4. Deletes the recognizer.
string workflowExample(AudioEvent* event, const string &modelId)
{
    const string recognizerIdBase = "example_recognizer";

    // make sure the recognizer ID is unique.
    const string recognizerId = MakeRecognizerName::sMakeRecognizerName.makeRecognizerName(recognizerIdBase);

    // instatiate a recognizer, using the model we've previously loaded
    //
    // we expect this to be a lightweight call, because all the heavy duty IO work has
    // been done in setting up the model.
    checkApiReturn(Api_NewDetector(recognizerId.c_str(), modelId.c_str()));

    string jsonVadEvents;
    // Push the audio event to the recognizer
    //
    // We recommend that the client push audio events as soon as they are available.
    // Buffering and pushing large chunks of audio all at once will lead to increased latency.

    checkDetectorReturn(Detector_PushEvent(recognizerId.c_str(), event), jsonVadEvents);

    // Delete the recognizer. We need to clean up memory associated with the recognizer.
    checkApiReturn(Api_DeleteDetector(recognizerId.c_str()));

    // we do not need this ID anymore, delete the recognizer ID from the client side records.
    MakeRecognizerName::sMakeRecognizerName.removeRecognizerName(recognizerId);
    return jsonVadEvents;
}


void getAudioEvent(AudioEvent &event)
{
    IfstreamPtr fstream = readfile("data/audio/woman1_wb.wav", std::ios::in | std::ios::binary);
    kaldi::WaveData wavData;
    wavData.Read(*fstream);
    wavToAudioEvent(wavData, event);
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

void test_api_working()
{
    const string realModel = "data/models/dummy.config";
    checkApiReturn(Api_NewModel(gRealModelId.c_str(), realModel.c_str()));
    string jsonEvents = workflowExample(&gEvent, gRealModelId);
    cout << "Vad events in JSON" << jsonEvents << endl;
    cobalt::VadEvents events;
    deserializeVadEvents(jsonEvents, events);
    printVadEvents(events);
}

int main()
{
    try
    {
        checkApiReturn(Api_RegisterLoggingCallback(&testLoggingCallback));

        // create global audio event.
        getAudioEvent(gEvent);
        test_api_working();
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
