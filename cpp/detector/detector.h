#ifndef COBALT_GUARD_DETECTOR_H_
#define COBALT_GUARD_DETECTOR_H_

#include "detector/vad_model.h"
#include <boost/make_shared.hpp>
#include "util/vad_events.h"
#include "api/api_types.h"
#include "feat/feature-mfcc.h"
#include "ivector/voice-activity-detection.h"

namespace cobalt
{
    class VadDetector
    {
    public:
        typedef boost::shared_ptr<VadDetector> Ptr;
        VadDetector(VadModel::Ptr model);

        // Function for pushing events, returns events as the second argument.
        // Note that events returned here may not be associated with audio pushed,
        // indeed, the events returned could have occurred before or after this chunk of audio, due to buffering
        //
        // Note that the number of events returned is 0-N.
        void pushEvents(AudioEvent *event, VadEvents& events);
        void shutdownDetector(){};

    private:
        boost::shared_ptr<kaldi::Mfcc> mMfcc;
        int mNumFramesDecoded;

        // false means in silence, true means in speech.
        bool mPreviousEndState;
    };
}

#endif
