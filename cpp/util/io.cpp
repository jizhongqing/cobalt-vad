#include "util/io.h"
#include "util/kaldi-io.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <boost/make_shared.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>
namespace fs = boost::filesystem;

namespace cobalt
{

    // helper function for getting filepath, using relative path
	string getFilepathParam(const string& filepath, const string& relativePath)
    {
        fs::path path(filepath);
        if (!fs::exists(path))
        {
            path = fs::path(relativePath) / path;
            if (!fs::exists(path))
            {
                std::stringstream ss;
                ss << "File not found at " << filepath << ", or " << path.string();
                throw std::runtime_error(ss.str());
            }
        }
        return path.string();
    }

	IfstreamPtr readfile(const std::string& filepath, std::ios_base::openmode mode)
    {
        IfstreamPtr fstream = boost::make_shared<std::ifstream>();
		string path = filepath;
		#ifdef _WIN32
		std::replace(path.begin(), path.end(), '/', '\\');
		if (path[0] == '\\')
		{
			path = "c:" + path;
		}
		#endif
        fstream->open(path.c_str(), mode);
        if (!fstream->is_open())
        {
			throw std::runtime_error("Could not open file: " + path);
        }
        return fstream;
    }

	OfstreamPtr writefile(const std::string& filepath, std::ios_base::openmode mode)
    {
        OfstreamPtr fstream = boost::make_shared<std::ofstream>();
        fstream->open(filepath.c_str(), mode);
        if (!fstream->is_open())
        {
            throw std::runtime_error("Could not open file: " + filepath);
        }
        return fstream;
    }

	std::pair<string, string> splitString(const std::string &input, const std::string &splitToken)
    {
        const size_t found=input.find_last_of(splitToken);
        if (found == string::npos)
        {
            throw std::runtime_error
                (string("Incompatible configuration format, expecting 'key=value', got ") +input);
        }
        string key = input.substr(0,found);
        string value = input.substr(found+1);
        boost::trim(key);
        boost::trim(value);
        return std::make_pair(key, value);
    }

	std::pair<string, string> splitFilename(const string& filepath)
    {
        // TODO: We may need to move to boost for this, as windows uses backslash for dirs.
        const size_t found=filepath.find_last_of("/");
        string parentPath;
        string filename = filepath;
        if (found != string::npos)
        {
            parentPath = filepath.substr(0,found);
            filename = filepath.substr(found+1);
        }
        return std::make_pair(parentPath, filename);
    }

    namespace cobaltjson
    {
        const string COBALT_OBJECT = "cobalt_object";
        const string VERSION = "version";
    }
    JsonValuePtr makeCobaltJson(const string& objectname, const string &version)
    {
        JsonValuePtr ret = boost::make_shared<Json::Value>();
        (*ret)[cobaltjson::COBALT_OBJECT] = objectname;
        (*ret)[cobaltjson::VERSION] = version;
        return ret;
    }

    bool endsWith(const string& full, const string& ending)
    {
        if (full.length() >= ending.length())
        {
            return (full.compare(full.length() - ending.length(), ending.length(), ending) == 0);
        }
        else
        {
            return false;
        }
    }

}
