#ifndef COBALT_GUARD_PARAMETER_H_
#define COBALT_GUARD_PARAMETER_H_
#include <string>
using std::string;

namespace cobalt
{
	typedef enum
	{
		FLOAT_TYPE,
		INT_TYPE,
		STRING_TYPE,
		PATH_TYPE,
		BOOL_TYPE,
	} ParameterType;

	struct Parameter
	{
		string value;
		string description;
		ParameterType type;
		Parameter(const std::string& value,
				  const std::string& description,
				  ParameterType type) :
		value(value), description(description), type(type)
		{}
		// default constructor for STL structures.
		Parameter() : type(INT_TYPE){};
	};
}



#endif /* CPP_CONFIG_PARAMETER_H_ */
