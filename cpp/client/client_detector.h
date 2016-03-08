#ifndef COBALT_GUARD_CLIENT_DETECTOR_H_
#define COBALT_GUARD_CLIENT_DETECTOR_H_
#include <string>
#include "client/client_utils.h"
#include "api/api.h"
#include "detector/audio_io.h"
namespace vad_client
{
    class ClientDetector
    {
    public:
        ClientDetector(const std::string& detectorIdBase, const std::string& modelId)
        : mDetectorId(MakeDetectorName::sMakeDetectorName.makeDetectorName(detectorIdBase))
        {
            checkApiReturn(Api_NewDetector(mDetectorId.c_str(), modelId.c_str()));
        }

        // pushes audio, and returns events (2nd param is the return).
        void pushAudio(const std::vector<short>& samples, cobalt::VadEvents &events)
        {
            AudioEvent event;
            event.audio = const_cast<short*>(&samples[0]);
            event.size = samples.size();
            string jsonVadEvents;
            checkDetectorReturn(Detector_PushEvent(mDetectorId.c_str(), &event), jsonVadEvents);
            deserializeVadEvents(jsonVadEvents, events);
        }

        void shutdownDetector()
        {
            checkApiReturn(Api_DeleteDetector(mDetectorId.c_str()));
            MakeDetectorName::sMakeDetectorName.removeDetectorName(mDetectorId);
        }

        std::string getDetectorId() const
        {
            return mDetectorId;
        }
    private:
        const std::string mDetectorId;

    };
}

#endif
