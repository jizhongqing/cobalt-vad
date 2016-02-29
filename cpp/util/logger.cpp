#include "util/logger.h"
#include <boost/make_shared.hpp>
#include "api/api_types.h"

namespace cobalt
{
	string loglevelToString(int level)
	{
		string ret;
		if (level == LOGLEVEL_DEBUG)
		{
			return "Debug";
		}
		if (level == LOGLEVEL_INFO)
		{
			return "Info";
		}
		if (level == LOGLEVEL_WARN)
		{
			return "Warn";
		}
		if (level == LOGLEVEL_ERROR)
		{
			return "Error";
		}
		if (level == LOGLEVEL_FATAL)
		{
			return "Fatal";
		}
		throw std::runtime_error("Unknown log level");
	}
	class StdoutLogger : public Logger
	{
	private:
		void logMessage(int level, const string& message) const
		{
			cout << "Log Level: " << loglevelToString(level) << ", LOG: " << message << endl;
		}
	};

	Logger::Ptr gLogger = boost::make_shared<StdoutLogger>();

	void logMessage(int level, const string& message)
	{
		gLogger->logMessage(level, message);
	}

	void logMessage(int level, const string& recognizerId, const string& message)
	{
		gLogger->logMessage(level, string("recognizer id: ") + recognizerId + " message: " + message);
	}

	void setLogger(Logger::Ptr logger)
	{
		gLogger = logger;
	}
}
