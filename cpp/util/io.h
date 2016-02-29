#ifndef COBALT_GUARD_UTIL_IO_H_
#define COBALT_GUARD_UTIL_IO_H_
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <vector>
#include <utility>

#include "json/json.h"
#include "util/usingstl.h"
namespace cobalt
{
    typedef boost::shared_ptr<std::ifstream> IfstreamPtr;
	IfstreamPtr readfile(const std::string& filepath, std::ios_base::openmode mode = std::ios::in);

    typedef boost::shared_ptr<std::ofstream> OfstreamPtr;
	OfstreamPtr writefile(const std::string& filepath, std::ios_base::openmode mode = std::ios::out);

	std::pair<string, string> splitString(const std::string &input, const std::string &splitToken);

    // utility function that splits a path to parent dir and filename.
	std::pair<string, string> splitFilename(const std::string& filepath);

    // helper function for getting filepath, using relative path
	string getFilepathParam(const string& filepath, const string& relativePath);

    typedef boost::shared_ptr<Json::Value> JsonValuePtr;

    JsonValuePtr makeCobaltJson(const string& objectname, const string& version);

    bool endsWith(const string& full, const string& ending);

}

#endif /* COBALT_GUARD_UTIL_IO_H_ */
