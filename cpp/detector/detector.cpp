#include "detector/detector.h"
#include "feat/feature-mfcc.h"
#include <iostream>
#include <vector>
#include "matrix/kaldi-matrix.h"
using namespace std;
namespace cobalt
{

    VadDetector::VadDetector(VadModel::Ptr model):
    mNumFramesDecoded(0), mPreviousEndState(false), mKaldiVadDetector(kaldi::VadEnergyOptions())
    {
        // TODO: fill out options from the model
        kaldi::MfccOptions mfcc_opts;
        mMfcc = boost::make_shared<kaldi::Mfcc>(mfcc_opts);
    }
    void VadDetector::pushEvents(AudioEvent *event, VadEvents& events)
    {
        // this turns off vtln
        kaldi::BaseFloat vtlnWarp = 1.0;
        kaldi::Vector<float> data;
        data.Resize(mAudioRemainder.Dim() + event->size);
        // probably a smarter way to do this.
        for (int i = 0; i < mAudioRemainder.Dim(); ++i)
        {
            data.Data()[i] = mAudioRemainder.Data()[i];
        }
        for (int i = 0; i < event->size; ++i)
        {
            data.Data()[i + mAudioRemainder.Dim()] = event->audio[i];
        }
        mAudioRemainder.Resize(0);
        kaldi::Matrix<kaldi::BaseFloat> features;
        mMfcc->Compute(data, vtlnWarp, &features, &mAudioRemainder);

        std::vector<bool> vadResults;
        mKaldiVadDetector.computeVadEnergy(features, &vadResults);
        const int numPrevFramesDecoded = mNumFramesDecoded;
        mNumFramesDecoded += vadResults.size();
        frameVadResultsToEvents(vadResults, events, numPrevFramesDecoded, mPreviousEndState);
        if (vadResults.size() > 0)
        {
            mPreviousEndState = vadResults.back();
        }
    }
}
