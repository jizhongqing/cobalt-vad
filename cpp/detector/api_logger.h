#ifndef COBALT_GUARD_API_LOGGER_H_
#define COBALT_GUARD_API_LOGGER_H_

#include "util/logger.h"
#include "util/usingstl.h"
#include "api/api_types.h"

namespace cobalt
{
    class ApiLogger : public Logger
    {
    public:
        ApiLogger(Logging_Callback* callback)
        {
            mCallback = callback;
        }
        void logMessage(int level, const string& message) const
        {
            if (*mCallback)
            {
                (*mCallback)(level, message.c_str());
            }
        }
    private:
        Logging_Callback* mCallback;
    };
}


#endif
