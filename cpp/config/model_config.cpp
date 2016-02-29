#include "config/model_config.h"
#include "util/io.h"

namespace cobalt
{
    namespace model_json
    {
        const string MODEL_JSON = "model_json";
        const string VERSION = "1";
        const string MODEL_PATH = "model_path";
        const string SUB_MODEL_ID = "sub_model_id";
        const string OVERRIDES = "overrides";
        const string PARAM = "param";
        const string VALUE = "value";
    }
    using namespace model_json;

    string serializeModelConfig(const ModelConfig& modelConfig)
    {
        JsonValuePtr root = makeCobaltJson(MODEL_JSON, VERSION);
        (*root)[MODEL_PATH] = modelConfig.modelPath;

        Json::Value overrides = Json::arrayValue;
        for (size_t i = 0; i < modelConfig.paramOverrides.size(); ++i)
        {
            Json::Value elem;
            elem[PARAM] = modelConfig.paramOverrides[i].paramName;
            elem[VALUE] = modelConfig.paramOverrides[i].overrideValue;
            overrides.append(elem);
        }
        (*root)[OVERRIDES] = overrides;
        Json::FastWriter fastWriter;
        fastWriter.omitEndingLineFeed();
        return fastWriter.write(*root);
    }

    void deserializeModelConfig(const string& jsonModelString, ModelConfig& modelConfig)
    {
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(jsonModelString, root))
        {
            // the path wasn't json, assume it is a path to a model.
            modelConfig.modelPath = jsonModelString;
            return;
        }
        if (root["cobalt_object"].asString() != MODEL_JSON)
        {
            throw std::runtime_error(string("Unexpected object_type in Json") + jsonModelString);
        }
        if (!root.isMember(MODEL_PATH))
        {
            throw std::runtime_error(string("No model path found, it is a required parameter."));
        }
        modelConfig.modelPath = root[MODEL_PATH].asString();
        if (root.isMember(OVERRIDES))
        {
            // an empty json to satisfy jsoncpp api.
            Json::Value empty;
            Json::Value overrides = root.get(OVERRIDES, empty);
            for (size_t i = 0; i < overrides.size(); ++i)
            {
         	Json::Value item = overrides.get(i, empty);
                string value = item[VALUE].asString();
                string param = item[PARAM].asString();
                ParamOverride paramOverride(param, value);
                modelConfig.paramOverrides.push_back(paramOverride);
            }
        }
        return;
    }
}
