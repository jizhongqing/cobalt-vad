#ifndef COBALT_GUARD_MODEL_H_
#define COBALT_GUARD_MODEL_H_

#include "ivector/voice-activity-detection.h"
#include "config/configset.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>
namespace cobalt
{
    class VadModel
    {
    public:
        typedef boost::shared_ptr<VadModel> Ptr;
        VadModel(const std::string& modelpath):
            mConfigset (boost::make_shared < ConfigSet > (modelpath))
        {
            loadVadConfig();
        };

        // getters
        const kaldi::VadEnergyOptions& getVadConfig()
        {
            return mVadConfig;
        }
        const std::string& getVadType()
        {
            return mVadType;
        }
        const ParametersMap& getParams()
        {
            return mConfigset->getParams();
        }

    private:
        // members
        ConfigSet::Ptr mConfigset;
        kaldi::VadEnergyOptions mVadConfig;
        std::string mVadType;

    private:
        // methods
        void loadVadConfig();
    };
}
#endif
