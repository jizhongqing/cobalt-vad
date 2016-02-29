#include "config/recognizer_construct.h"
#include "config/asr_configset.h"
#include "config/decoder_configs.h"
#include "config/endpoint_config.h"
#include "config/model_config.h"
#include "util/usingstl.h"
#include "util/ensure.h"
#include <boost/make_shared.hpp>

using namespace cobalt;

string typeToString(ParameterType type)
{
	if (type == FLOAT_TYPE) return "float";
	if (type == INT_TYPE) return "int";
	if (type == STRING_TYPE) return "string";
	if (type == PATH_TYPE) return "path";
    if (type == BOOL_TYPE) return "bool";
	throw std::runtime_error("Unknown type");
}

void printParameterMap(const ParametersMap& params)
{
	for (ParametersMap::const_iterator it = params.begin();
	     it != params.end();
	     ++it)
	{
		cout << "key: " << it->first
			<< ", value: " << it->second.value
			<< ", description: " << it->second.description
			<< ", type: " << typeToString(it->second.type) << endl;
	}
}

void test_default_config()
{
	cout << endl << endl << "Begin test_default_config()" << endl;
	// an empty config file should lead to the default parameters.
	ConfigSet::Ptr config = boost::make_shared<AsrConfigSet>("data/models/dummy_config/empty.config",
	                                                      std::vector<ParamOverride>());
	const ParametersMap& params = config->getParams();
	printParameterMap(params);
	cout << endl << endl << "Finish test_default_config()" << endl;
}

void test_user_config()
{
	cout << endl << endl << "Begin test_user_config()" << endl;
	// a 'realistic' config file.
	ConfigSet::Ptr config = boost::make_shared<AsrConfigSet>("data/models/dummy_config/example.config",
	                                                       std::vector<ParamOverride>());
	const ParametersMap& params = config->getParams();
	printParameterMap(params);
	cout << endl << endl << "Finish test_user_config()" << endl;
}

void test_override_config()
{
    cout << endl << endl << "Begin test_override_config()" << endl;

    std::vector<ParamOverride> overrides;

    const string decoderType = "override.decoder_type";
    const string decoderTypeParam = "search.decoder_type";
    const string decodeFstPath = "override.decode_fst_filepath";
    const string decodeFstPathParam = "search.decode_fst_filepath";

    overrides.push_back(ParamOverride(decoderTypeParam, decoderType));
    overrides.push_back(ParamOverride(decodeFstPathParam, decodeFstPath));
    // a 'realistic' config file.
    ConfigSet::Ptr config = boost::make_shared<AsrConfigSet>("data/models/dummy_config/example.config", overrides);
    const ParametersMap& params = config->getParams();
    cobalt_ensure(params.find(decoderTypeParam)->second.value == decoderType);
    cobalt_ensure(params.find(decodeFstPathParam)->second.value == decodeFstPath);

    printParameterMap(params);

    cout << endl << endl << "Finish test_override_config()" << endl;
}

void test_parent_path()
{
	cout << endl << endl << "Begin test_parent_path()" << endl;
	// an empty config file should lead to the default parameters.
	const string filepath = "data/models/dummy_config/empty.config";
	ConfigSet::Ptr config = boost::make_shared<AsrConfigSet>(filepath, std::vector<ParamOverride>());
	cout << "Parent path of " << filepath << " is " << config->getParentPath() << endl;
	cout << endl << endl << "Finish test_parent_path()" << endl;
}

// helper print funtions
void printConfig(const kaldi::LatticeFasterDecoderConfig& config);
void printConfig(const DecoderConfigs &config);
void printConfig(const kaldi::FasterDecoderOptions& config);

void test_decoder_configs()
{
	cout << endl << endl << "Begin test_decoder_configs()" << endl;

	cout << endl << "Test conversion of LatticeFasterDecoderConfig to DecoderConfig and back" << endl;
	kaldi::LatticeFasterDecoderConfig latticeConfig;
	cout << endl << "Default LatticeFasterDecoderConfig" << endl;
	printConfig(latticeConfig);

	DecoderConfigs decoderConfigLat = fromLatticeFasterConfig(latticeConfig);
	cout << endl << "LatticeFasterDecoderConfig to DecoderConfigs" << endl;
	printConfig(decoderConfigLat);

	kaldi::LatticeFasterDecoderConfig latticeConvertBack = toLatticeFasterConfig(decoderConfigLat);
	cout << endl << "LatticeFasterDecoderConfig from DecoderConfigs" << endl;
	printConfig(latticeConvertBack);

	cout << endl << "Test conversion of FasterDecoderOptions to DecoderConfig and back" << endl;
	kaldi::FasterDecoderOptions fasterConfig;
	cout << endl << "Default FasterDecoderOptions" << endl;
	printConfig(fasterConfig);

	DecoderConfigs decoderConfigViterbi = fromFasterConfig(fasterConfig);
	cout << endl << "FasterDecoderOptions to DecoderConfigs" << endl;
	printConfig(decoderConfigViterbi);

	kaldi::FasterDecoderOptions fasterConvertBack = toFasterConfig(decoderConfigViterbi);
	cout << endl << "FasterDecoderOptions from DecoderConfigs" << endl;
	printConfig(fasterConvertBack);

	cout << endl << endl << "Finish test_decoder_configs()" << endl;
}

void printEndpointRule(const EndpointRule &rule);

void test_endpoint_config();

void test_kaldi_endpoint_config();

void test_model_config();

void transcriptEqual(const std::vector<std::string>& left, const std::vector<std::string>& right)
{
    if (left.size() != right.size())
    {
        throw std::runtime_error("transcript sizes not equal.");
    }
    for (size_t i = 0; i < left.size(); ++i)
    {
        if (left[i] != right[i])
        {
            throw std::runtime_error("transcript not equal.");
        }
    }
}

void transcriptsEqual(const std::vector<std::vector<std::string> >& left, const std::vector<std::vector<std::string> >& right)
{
    if (left.size() != right.size())
    {
        throw std::runtime_error("transcripts sizes not equal.");
    }
    for (size_t i = 0; i < left.size(); ++i)
    {
        transcriptEqual(left[i], right[i]);
    }
}

void testRecognizerContextSerialization(const RecognizerConstruct &construct)
{
    std::string constructJson = makeRecognitionConstruct(construct);

    cout << "serialized construct: " << constructJson << endl;

    RecognizerConstruct::Ptr reserializedConstruct = readRecognitionConstruct(constructJson);
    cobalt_ensure(reserializedConstruct->modelDomain == construct.modelDomain);
    cobalt_ensure(reserializedConstruct->modelsetId == construct.modelsetId);
    cobalt_ensure(reserializedConstruct->recognizerType == construct.recognizerType);
    transcriptsEqual(construct.transcripts, reserializedConstruct->transcripts);
}

void test_recognizer_construct()
{
    cout << endl << endl << "Begin test_recognizer_construct()" << endl;

    cout << "test a recognizer construct with no recognizer-type" << endl;
    RecognizerConstruct construct;
    construct.modelDomain = "domain1";
    construct.modelsetId = "model1";

    testRecognizerContextSerialization(construct);
    cout << "test a recognizer construct with recognizer-type" << endl;
    construct.recognizerType = "asr";
    testRecognizerContextSerialization(construct);


    cout << "test a recognizer construct with transcripts" << endl;
    vector<string> transcript1;
    transcript1.push_back("lets");
    transcript1.push_back("recognize");
    transcript1.push_back("speech");

    vector<string> transcript2;
    transcript2.push_back("hello");
    transcript2.push_back("world");
    construct.transcripts.push_back(transcript1);
    construct.transcripts.push_back(transcript2);
    testRecognizerContextSerialization(construct);

    RecognizerConstruct::Ptr onlyModelsetConstruct = readRecognitionConstruct("only_modelset");
    cobalt_ensure(onlyModelsetConstruct->modelDomain == "");
    cobalt_ensure(onlyModelsetConstruct->modelsetId == "only_modelset");
    cobalt_ensure(onlyModelsetConstruct->recognizerType == "");

    cout << endl << endl << "End test_recognizer_construct()" << endl;
}

int main()
{
	try
	{
		test_default_config();
		test_user_config();
		test_override_config();
		test_parent_path();
		test_decoder_configs();
		test_endpoint_config();
		test_kaldi_endpoint_config();
		test_model_config();
		test_recognizer_construct();
	}
	catch (std::runtime_error &e)
	{
		cout << "Caught C++ runtime error: " << e.what() << endl;
		return 1;
	}
	catch ( ... )
	{
		cout << "Caught unknown error!!";
		return 2;
	}
	return 0;
}

bool compareModelConfig(const ModelConfig& left, const ModelConfig& right)
{
    if (left.modelPath != right.modelPath)
    {
        return false;
    }
    if (left.paramOverrides.size() != right.paramOverrides.size())
    {
        return false;
    }
    for (size_t i = 0; i < left.paramOverrides.size(); ++i)
    {
        ParamOverride orLeft = left.paramOverrides[i];
        ParamOverride orRight = right.paramOverrides[i];
        if (orLeft.paramName != orRight.paramName || orLeft.overrideValue != orRight.overrideValue)
        {
            return false;
        }
    }
    return true;
}

// tests serialization and deserializtion.
void testModelConfigSerialization(const ModelConfig &config)
{
    cout << endl << "testing round-trip serialization" << endl;
    const string serializedConfig = serializeModelConfig(config);
    cout << "serialized model config: " << serializedConfig << endl;
    ModelConfig deserializedConfig;
    deserializeModelConfig(serializedConfig, deserializedConfig);
    if (!compareModelConfig(config, deserializedConfig))
    {
        throw std::runtime_error("Original and deserialized configs not equal.");
    }

    cout << endl << "end testing round-trip serialization" << endl;
}

void test_model_config()
{
    cout << endl << endl << "Begin test_model_config()" << endl;
    ModelConfig config;
    config.modelPath = "dummy_path";
    testModelConfigSerialization(config);

    {
        ParamOverride override("param1", "value1");
        config.paramOverrides.push_back(override);
    }
    testModelConfigSerialization(config);

    {
        ParamOverride override("param2", "value2");
        config.paramOverrides.push_back(override);
    }
    testModelConfigSerialization(config);

    cout << endl << "Test that we handle a  non-json model string." << endl;
    const string nonJsonModel = "not_a_json";
    ModelConfig configFromNoJson;
    deserializeModelConfig(nonJsonModel, configFromNoJson);
    cobalt_ensure(configFromNoJson.modelPath == nonJsonModel);
    if(configFromNoJson.paramOverrides.size() != 0)
    {
        throw std::runtime_error("Expected no override params");
    }
    cout << endl << endl << "End test_model_config()" << endl;
}


void printEndpointRule(const EndpointRule &rule)
{
    cout << endl << "EndPointRule: " << endl;

    cout << "rule.maxRelativeCost: " << rule.maxRelativeCost << endl;
    cout << "rule.minUtteranceLengthFrames: " << rule.minUtteranceLengthFrames << endl;
    cout << "rule.mustContainSilence: " << rule.mustContainSilence << endl;
    cout << "rule.mustReachFinal: " << rule.mustReachFinal << endl;
    cout << "rule.trailingSilenceFrames: " << rule.trailingSilenceFrames << endl;
}

void printEndpointConfig(const EndpointConfig& config)
{
    cout << endl << "EndpointConfig: " << endl;

    cout << "config.silencePhoneString: " << config.silencePhoneString << endl;
    cout << "config.type: " << config.type << endl;
    printEndpointRule(config.rule);
}

void test_endpoint_config()
{
    cout << endl << endl << "Begin test_endpoint_config()" << endl;

    ConfigSet::Ptr config = boost::make_shared<AsrConfigSet>("data/models/dummy_config/example.config",
                                                          std::vector<ParamOverride>());
    const ParametersMap& params = config->getParams();

    EndpointRule rule = getRuleFromConfigMap(params);
    printEndpointRule(rule);

    EndpointConfig endptConfig = getEndpointConfigFromConfigMap(params);
    printEndpointConfig(endptConfig);

    cout << endl << endl << "Finish test_endpoint_config()" << endl;
}

void printOnlineEndpointRule(const kaldi::OnlineEndpointRule & rule)
{
    cout << endl << "kaldi::OnlineEndpointRule: " << endl;
    cout << "must_contain_nonsilence: " << rule.must_contain_nonsilence << endl;
    cout << "min_trailing_silence: " << rule.min_trailing_silence << endl;
    cout << "max_relative_cost: " << rule.max_relative_cost << endl;
    cout << "min_utterance_length: " << rule.min_utterance_length << endl;
}

void printKaldiOnlineEndpointConfig(const kaldi::OnlineEndpointConfig &config)
{
    cout << endl << "kaldi::OnlineEndpointConfig: " << endl;
    cout << "config.silence_phones: " << config.silence_phones << endl;
    cout << "Rule 1: " << endl;
    printOnlineEndpointRule(config.rule1);
    cout << endl << "Rule 2: " << endl;
    printOnlineEndpointRule(config.rule2);
    cout << endl << "Rule 3: " << endl;
    printOnlineEndpointRule(config.rule3);
    cout << endl << "Rule 4: " << endl;
    printOnlineEndpointRule(config.rule4);
    cout << endl << "Rule 5: " << endl;
    printOnlineEndpointRule(config.rule5);
}

void test_kaldi_endpoint_config()
{
    cout << endl << endl << "Begin test_kaldi_endpoint_config()" << endl;

    cout << "This is the default kaldi online endpointing config." << endl;
    kaldi::OnlineEndpointConfig config;
    printKaldiOnlineEndpointConfig(config);

    cout << endl << "Use a non-default kaldi online endpointing config." << endl;
    const string kaldiOnlineEndptConfigPath = "data/models/dummy_config/kaldi-online-endpt.config";
    kaldi::ParseOptions po("endpoint config parser");
    config.Register(&po);
    po.ReadConfigFile(kaldiOnlineEndptConfigPath);
    printKaldiOnlineEndpointConfig(config);
    cout << endl << endl << "Finish test_kaldi_endpoint_config()" << endl;
}

void printConfig(const kaldi::LatticeFasterDecoderConfig& config)
{
	cout << "kaldi::LatticeFasterDecoderConfig" << endl;
	cout << "ret.beam " <<  config.beam << endl;
	cout << "ret.beam_delta " << config.beam_delta<< endl;
	cout << "ret.hash_ratio " << config.hash_ratio<< endl;
	cout << "ret.lattice_beam  " << config.lattice_beam<< endl;
	cout << "ret.max_active " << config.max_active<< endl;
	cout << "ret.min_active " << config.min_active<< endl;
	cout << "ret.prune_interval " << config.prune_interval<< endl;
	cout << "ret.prune_scale " << config.prune_scale<< endl;
}

void printConfig(const DecoderConfigs &config)
{
	cout << "DecoderConfig" << endl;
	cout << "ret.beam " <<  config.beam << endl;
	cout << "ret.beam_delta " << config.beam_delta<< endl;
	cout << "ret.hash_ratio " << config.hash_ratio<< endl;
	cout << "ret.lattice_beam  " << config.lattice_beam<< endl;
	cout << "ret.max_active " << config.max_active<< endl;
	cout << "ret.min_active " << config.min_active<< endl;
	cout << "ret.prune_interval " << config.prune_interval<< endl;
	cout << "ret.prune_scale " << config.prune_scale<< endl;
}

void printConfig(const kaldi::FasterDecoderOptions& config)
{
	cout << "kaldi::FasterDecoderOptions" << endl;
	cout << "ret.beam " <<  config.beam << endl;
	cout << "ret.beam_delta " << config.beam_delta<< endl;
	cout << "ret.hash_ratio " << config.hash_ratio<< endl;
	cout << "ret.max_active " << config.max_active<< endl;
	cout << "ret.min_active " << config.min_active<< endl;
}




