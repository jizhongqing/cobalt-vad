#include "feat/feature-mfcc.h"
#include "config/model_config.h"
#include "config/configset.h"
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
    ConfigSet::Ptr config = boost::make_shared<ConfigSet>("data/models/dummy_config/empty.config");
    const ParametersMap& params = config->getParams();
    printParameterMap(params);
    cout << endl << endl << "Finish test_default_config()" << endl;
}

void test_user_config()
{
    cout << endl << endl << "Begin test_user_config()" << endl;
    // a 'realistic' config file.
    ConfigSet::Ptr config = boost::make_shared<ConfigSet>("data/models/dummy_config/example.config");
    const ParametersMap& params = config->getParams();
    printParameterMap(params);
    cout << endl << endl << "Finish test_user_config()" << endl;
}

void test_parent_path()
{
    cout << endl << endl << "Begin test_parent_path()" << endl;
    // an empty config file should lead to the default parameters.
    const string filepath = "data/models/dummy_config/empty.config";
    ConfigSet::Ptr config = boost::make_shared<ConfigSet>(filepath);
    cout << "Parent path of " << filepath << " is " << config->getParentPath() << endl;
    cout << endl << endl << "Finish test_parent_path()" << endl;
}

int main()
{
    try
    {
        test_default_config();
        test_user_config();
        test_parent_path();
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
