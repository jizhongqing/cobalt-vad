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


// Steady-state detection. We've already setup callbacks and a model, we are ready
// to detect speech events.
//
// This function
// 1. Instantiate a detector.
// 2. Pushes an audio event to the detector, and get events, if any.
// 3. Deletes the detector.
string workflowExample(AudioEvent* event, const string &modelId)
{
    const string detectorIdBase = "example_detector";

    // make sure the detector ID is unique.
    const string detectorId = MakeDetectorName::sMakeDetectorName.makeDetectorName(detectorIdBase);

    // instatiate a detector, using the model we've previously loaded
    //
    // we expect this to be a lightweight call, because all the heavy duty IO work has
    // been done in setting up the model.
    checkApiReturn(Api_NewDetector(detectorId.c_str(), modelId.c_str()));

    string jsonVadEvents;
    // Push the audio event to the detector
    //
    // We recommend that the client push audio events as soon as they are available.
    // Buffering and pushing large chunks of audio all at once will lead to increased latency.

    checkDetectorReturn(Detector_PushEvent(detectorId.c_str(), event), jsonVadEvents);

    // Delete the detector. We need to clean up memory associated with the detector.
    checkApiReturn(Api_DeleteDetector(detectorId.c_str()));

    // we do not need this ID anymore, delete the detector ID from the client side records.
    MakeDetectorName::sMakeDetectorName.removeDetectorName(detectorId);
    return jsonVadEvents;
}


void getAudioEvent(AudioEvent &event)
{
    IfstreamPtr fstream = readfile("data/audio/woman1_wb.wav", std::ios::in | std::ios::binary);
    kaldi::WaveData wavData;
    wavData.Read(*fstream);
    wavToAudioEvent(wavData, event);
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
