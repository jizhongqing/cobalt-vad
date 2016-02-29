#ifndef CPP_UTIL_LOGGER_H_
#define CPP_UTIL_LOGGER_H_
#include "util/usingstl.h"
#include "api/api_types.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace cobalt
{

	class Logger
	{
	public:
		typedef boost::shared_ptr<Logger> Ptr;
		virtual void logMessage(int level, const string& message) const = 0;
		virtual ~Logger(){};
	};

	class EmptyLogger : public Logger
	{
		void logMessage(int level, const string& message) const
		{
			// do nothing!
		}
	};
	void logMessage(int level, const string& message);

	void logMessage(int level, const string& recognizerId, const string& message);

	// entry point to set a logger (may be called by api classes)
	void setLogger(Logger::Ptr apiLogger);

	// level to string
	string loglevelToString(int level);
}

#endif /* CPP_UTIL_LOGGER_H_ */
