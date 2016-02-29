#ifndef COBALT_GUARD_MODEL_CONFIG_H_
#define COBALT_GUARD_MODEL_CONFIG_H_

#include "util/usingstl.h"
namespace cobalt
{
    struct ParamOverride
    {
        string paramName;
        string overrideValue;
        ParamOverride(const string &paramName,
                const string& overrideValue)
        :paramName(paramName), overrideValue(overrideValue)
        {}
        // default constructor for stl containers
        ParamOverride(){};
    };

    struct ModelConfig
    {
        string modelPath;
        vector<ParamOverride> paramOverrides;
    };

    /**
     * Create a model JSON from a potential JSON string
     */
    void deserializeModelConfig(const string& jsonModelString, ModelConfig& modelConfig);

    /**
     * Converts model JSON from parameters.
     */
    string serializeModelConfig(const ModelConfig& modelConfig);
}

#endif /* COBALT_GUARD_MODEL_CONFIG_H_ */
