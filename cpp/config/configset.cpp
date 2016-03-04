#include "config/configset.h"

#include "util/usingstl.h"
#include "util/io.h"
#include "util/logger.h"

#include <sstream>
#include <boost/algorithm/string.hpp>

namespace cobalt
{
    ConfigSet::ConfigSet(const string& configfile)
    : mParentPath(splitFilename(configfile).first)
    {
        addDefaultConfigs();
        parseConfigFile(configfile);
    }

    void ConfigSet::addDefaultConfigs()
    {
        addDefaultParam("fe.vad_type", "kaldi-vad", \
                        "String for the type of VAD", STRING_TYPE);
        addDefaultParam("fe.vad_energy_threshold", "5.0", \
                        "Constant term in energy threshold for MFCC0 for VAD "
                        "(also see fe.vad_energy_mean_scale)", FLOAT_TYPE);
        addDefaultParam("fe.vad_energy_mean_scale", "0.5", \
                        "If this is set to s, to get the actual threshold "
                        "we let m be the mean log-energy of the file, and "
                        "use s*m + vad-energy-threshold", FLOAT_TYPE);
        addDefaultParam("fe.vad_frame_context", "5", \
                        "Number of frames of context on each side of central frame, "
                        "in window for which energy is monitored", INT_TYPE);
        addDefaultParam("fe.vad_proportion_threshold", "0.6", \
                        "Parameter controlling the proportion of frames within "
                        "the window that need to have more energy than the "
                        "threshold", FLOAT_TYPE);
    }

    const ParametersMap& ConfigSet::getParams() const
    {
        return mParams;
    }

    const string& ConfigSet::getParentPath() const
    {
        return mParentPath;
    }

    void ConfigSet::addDefaultParam(const string& paramName,
                  const string& value,
                  const string& description,
                  ParameterType type)
    {
        Parameter param(value, description, type);
        if (mParams.find(paramName) != mParams.end())
        {
            throw std::runtime_error("Attempting to add existing default param " + paramName);
        }
        mParams[paramName] = param;
    }

    // add params specified by the user, paramName must be known set of params, already added by
    // the addDefaultConfigs() function.
    void ConfigSet::addUserSpecifiedParam(const string& paramName,
                  const string& value)
    {
        ParametersMap::iterator it = mParams.find(paramName);
        if (it == mParams.end())
        {
            throw std::runtime_error("Attempting to add an unknown parameter " + paramName);
        }
        Parameter param(value, it->second.description, it->second.type);
        it->second = param;
    }
    // add params specified by overrides, paramName must be known set of params, already added by
    // the addDefaultConfigs() function.
    void ConfigSet::addOverrideParams(const std::vector<ParamOverride>& overrides)
    {
        for (size_t i = 0; i < overrides.size(); ++i)
        {
            std::stringstream ss;
            ss << "Overriding parameter: " << overrides[i].paramName << " as " << overrides[i].overrideValue;
            logMessage(LOGLEVEL_INFO, ss.str());
            addUserSpecifiedParam(overrides[i].paramName, overrides[i].overrideValue);
        }
    }

    string ConfigSet::getPathParam(const string& parameterName) const
    {
        Parameter param = mParams.find(parameterName)->second;
        if (param.type != PATH_TYPE)
        {
            throw std::runtime_error(string("Calling getPathParam for incompatible parameter ") + parameterName);
        }
        return getFilepathParam(param.value, mParentPath);
    }

    void ConfigSet::parseConfigFile(const std::string& configfile)
    {
        logMessage(LOGLEVEL_INFO, string("Reading configuration file ") + configfile);
        IfstreamPtr fstream = readfile(configfile);
        string line;
        while (std::getline(*fstream,line))
        {
            boost::trim(line);
            if (line == "" || line.at(0) == '#')
            {
                // do not parse empty or comment lines.
                continue;
            }
            std::pair<string, string> keyValue= splitString(line, "=");
            addUserSpecifiedParam(keyValue.first, keyValue.second);
        }

    }
}

