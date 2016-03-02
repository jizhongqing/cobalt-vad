#ifndef COBALT_GUARD_CONFIGSET_H_
#define COBALT_GUARD_CONFIGSET_H_

#include "config/parameter.h"
#include "config/model_config.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

using std::string;
namespace cobalt
{
    typedef std::map<std::string, Parameter> ParametersMap;
    class ConfigSet
    {
    public:
        typedef boost::shared_ptr<ConfigSet> Ptr;

        ConfigSet(const std::string& configfile);

        const ParametersMap& getParams() const;

        const string& getParentPath() const;

        string getPathParam(const string& parameterName) const;

        virtual ~ConfigSet(){}

    protected:
        // methods
        virtual void addDefaultConfigs() = 0;
        void parseConfigFile(const string& configfile);
        void addDefaultParam(const string& paramName,
                      const string& value,
                      const string& description,
                      ParameterType type);

        void addUserSpecifiedParam(const string& paramName,
                                   const string& value);

        void addOverrideParams(const std::vector<ParamOverride>& overrides);

    protected:
        // members
        ParametersMap mParams;
        // parent path of the config file (sent in via constructor)
        // this will be required to read nested structures.
        const string mParentPath;
    };
}

#endif /* COBALT_GUARD_CONFIGSET_H_ */
