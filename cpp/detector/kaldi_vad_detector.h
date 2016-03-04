#ifndef COBALT_GUARD_KALDI_VAD_DETECTOR_H_
#define COBALT_GUARD_KALDI_VAD_DETECTOR_H_

#include "ivector/voice-activity-detection.h"

namespace cobalt
{

    class KaldiVadDetector
    {
    public:
        KaldiVadDetector(const kaldi::VadEnergyOptions &vadOptions)
        : mVadOptions(vadOptions), mLogEnergySumAllFrames(0.0), mNumFramesAboveThreshold(0), mDenCount(0) {}

        void computeVadEnergy(const kaldi::MatrixBase<kaldi::BaseFloat> &inputFeatures,
                              std::vector<bool> *outputVoiced)
        {
            const int numFrames = inputFeatures.NumRows();
            if (inputFeatures.NumCols() == 0)
            {
                return;
            }
            kaldi::Vector<kaldi::BaseFloat> logEnergy(numFrames);
            logEnergy.CopyColFromMat(inputFeatures, 0); // column zero is log-energy.
            const size_t previousFramesSeen = mLogEnergyAllFrames.size();
            for (int i = 0; i < logEnergy.Dim(); ++i)
            {
                mLogEnergyAllFrames.push_back(logEnergy(i));
                mLogEnergySumAllFrames += logEnergy(i);
            }
            kaldi::BaseFloat energyThreshold = mVadOptions.vad_energy_threshold;
            int totalFramesSeen = mLogEnergyAllFrames.size();
            if (mVadOptions.vad_energy_mean_scale != 0.0)
            {
                KALDI_ASSERT(mVadOptions.vad_energy_mean_scale > 0.0);
                energyThreshold += mVadOptions.vad_energy_mean_scale * mLogEnergySumAllFrames / totalFramesSeen;
            }

            KALDI_ASSERT(mVadOptions.vad_frames_context >= 0);
            KALDI_ASSERT(mVadOptions.vad_proportion_threshold > 0.0 &&
                         mVadOptions.vad_proportion_threshold < 1.0);
            // iterate over new frames seen.
            for (int frame = previousFramesSeen; frame < totalFramesSeen; frame++)
            {
                int context = mVadOptions.vad_frames_context;
                int numFramesAboveThreshold = 0;
                int denCount = 0;
                // moving window of frames
                for (int windowedFrame = frame - context; windowedFrame <= frame + context; windowedFrame++)
                {
                    // accumulate stats when moving window within range
                    if (windowedFrame >= 0 && windowedFrame < totalFramesSeen)
                    {
                        denCount++;
                        if (mLogEnergyAllFrames[frame] > energyThreshold)
                        {
                            numFramesAboveThreshold++;
                        }
                    }
                 }
                 const bool frameInSpeech = (numFramesAboveThreshold >= denCount * mVadOptions.vad_proportion_threshold);
                 outputVoiced->push_back(frameInSpeech);
            }
        }

    private:
        const kaldi::VadEnergyOptions mVadOptions;
        std::vector<float> mLogEnergyAllFrames;
        float mLogEnergySumAllFrames;
        int mNumFramesAboveThreshold;
        int mDenCount;
    };
}




#endif
