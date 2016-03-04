#include "detector/vad_model.h"
#include "config/configset.h"
#include "util/logger.h"
#include "util/io.h"
#include "api/api_types.h"
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

using namespace kaldi;
namespace cobalt
{
    void
    VadModel::loadVadConfig ()
    {
        const ParametersMap& params = mConfigset->getParams ();

        mVadType = params.find ("fe.vad_type")->second.value;

        mVadConfig.vad_energy_mean_scale = boost::lexical_cast<float> (
                params.find ("fe.vad_energy_threshold")->second.value);
        mVadConfig.vad_energy_mean_scale = boost::lexical_cast<float> (
                params.find ("fe.vad_energy_mean_scale")->second.value);
        mVadConfig.vad_frames_context = boost::lexical_cast<int> (
                params.find ("fe.vad_frame_context")->second.value);
        mVadConfig.vad_proportion_threshold = boost::lexical_cast<float> (
                params.find ("fe.vad_proportion_threshold")->second.value);
    }
}


