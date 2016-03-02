#include "api.h"
#include "detector/detector_api.h"
#include <string>
#include <stdexcept>
#include <cstring>
#include <stdlib.h>

#include <boost/shared_ptr.hpp>

using namespace cobalt;


ApiReturn successRet()
{
    ApiReturn ret;
    // zero for success
    ret.error = 0;
    return ret;
}

DetectorReturn successRet(const string& jsonStr)
{
        DetectorReturn ret;
        ret.error = 0;
        ret.resultsJson = (char *) malloc((jsonStr.size()+1)*sizeof(char));
        strcpy(ret.resultsJson, jsonStr.c_str());
        return ret;
}

template<class ReturnType>
ReturnType errorRet(const string& msg)
{
    ReturnType ret;
    // non-zero for fail
    ret.error = 1;
    ret.errorMessage = (char *) malloc((msg.size()+1)*sizeof(char));
    strcpy(ret.errorMessage, msg.c_str());
    return ret;
}


#define EXCEPTION_TO_API_RETURN \
    catch (std::runtime_error &e) \
    { \
        return errorRet<ApiReturn>(e.what()); \
    } \
    catch ( ... ) \
    { \
        return errorRet<ApiReturn>("Unknown Error"); \
    } \

#define EXCEPTION_TO_DETECTOR_RETURN \
    catch (std::runtime_error &e) \
    { \
        return errorRet<DetectorReturn>(e.what()); \
    } \
    catch ( ... ) \
    { \
        return errorRet<DetectorReturn>("Unknown Error"); \
    } \


ApiReturn Api_NewModel(CobaltString modelIdC, CobaltString modelPathC)
{
        try
        {
            DetectorApi::sDetectorApi.Api_NewModel(modelIdC, modelPathC);
            return successRet();
        }
        EXCEPTION_TO_API_RETURN

}


ApiReturn Api_DeleteModel(CobaltString modelIdC)
{
    try
    {
        DetectorApi::sDetectorApi.Api_DeleteModel(modelIdC);
        return successRet();
    }
    EXCEPTION_TO_API_RETURN
}

ApiReturn Api_NewDetector(CobaltString recognizerIdC, CobaltString modelIdC)
{
    try
    {
        DetectorApi::sDetectorApi.Api_NewDetector(recognizerIdC, modelIdC);
        return successRet();
    }
    EXCEPTION_TO_API_RETURN
}

ApiReturn Api_DeleteDetector(CobaltString recognizerIdC)
{
    try
    {
        DetectorApi::sDetectorApi.Api_DeleteDetector(recognizerIdC);
        return successRet();
    }
    EXCEPTION_TO_API_RETURN
}

DetectorReturn Detector_PushEvent(CobaltString recognizerIdC, AudioEvent* event)
{
    try
    {
        string serializedEvents;
        DetectorApi::sDetectorApi.Detector_PushEvent(recognizerIdC, event, serializedEvents);
        return successRet(serializedEvents);
    }
    EXCEPTION_TO_DETECTOR_RETURN
}

ApiReturn Api_RegisterLoggingCallback(Logging_Callback callback)
{
    try
    {
        DetectorApi::sDetectorApi.Api_RegisterLoggingCallback(callback);
        return successRet();
    }
    EXCEPTION_TO_API_RETURN
}

void Api_Clear(Memory clear)
{
    if (clear != NULL)
    {
        free(clear);
    }
}
