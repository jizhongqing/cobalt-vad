#include "detector/detector.h"
#include "feat/feature-mfcc.h"
#include <iostream>
using namespace std;
namespace cobalt
{
    VadDetector::VadDetector(VadModel::Ptr model):
    mNumFramesDecoded(0), mPreviousEndState(false)
    {
        // TODO: fill out options from the model
        kaldi::MfccOptions mfcc_opts;
        mMfcc = boost::make_shared<kaldi::Mfcc>(mfcc_opts);
    }
    void VadDetector::pushEvents(AudioEvent *event, VadEvents& events)
    {
        // this turns off vtln
        kaldi::BaseFloat vtlnWarp = 1.0;
        // TODO, use real event.
        kaldi::Vector<float> data;
        data.Resize(event->size);
        for (int i = 0; i < event->size; ++i)
        {
            data.Data()[i] = event->audio[i];
        }
        kaldi::Matrix<kaldi::BaseFloat> features;
        mMfcc->Compute(data, vtlnWarp, &features, NULL);

        kaldi::Vector<kaldi::BaseFloat> vadResult(features.NumRows());
        //TODO: get this from the model.
        kaldi::VadEnergyOptions opts;
        kaldi::ComputeVadEnergy(opts, features, &vadResult);
        const int mNumPrevFramesDecoded = mNumFramesDecoded;
        mNumFramesDecoded += vadResult.Dim();

        if (vadResult.Dim() > 0)
        {
            mPreviousEndState = kaldiVadToBool(vadResult(vadResult.Dim()-1));
        }
        frameVadResultsToEvents(vadResult, events, mNumPrevFramesDecoded, mPreviousEndState);

        /*
        cout << "vadResults" << endl;
        for (int i = 0; i < vadResult.Dim(); ++i)
        {
            cout << vadResult(i) << " ";
        }

        cout << endl;
        */

    }
}
