#ifndef COBALT_GUARD_API_TYPES_H_
#define COBALT_GUARD_API_TYPES_H_

/**
 * @file api_types.h
 * types used by VAD api
 */


// C types used by the cobalt C api.
typedef const char* CobaltString;
typedef char* Memory;

// Logging Levels, a subset of log4j levels
#define LOGLEVEL_TRACE 0 // most fine grained log level available
#define LOGLEVEL_DEBUG 1 // fine grained messages to aid debugging
#define LOGLEVEL_INFO 2  // coarse grained information messages, mostly event based
#define LOGLEVEL_WARN 3  // warning messages, potentially harmful situations
#define LOGLEVEL_ERROR 4 // error events but application will keep going
#define LOGLEVEL_FATAL 5 // application will abort shortly


// logging callback
typedef void (*Logging_Callback)(int loglevel, CobaltString msg);

// common return struct for API function calls
typedef struct ApiReturn
{
    // 0 means success, non-0 means failures
    int error;
    
    // if error is non-0, errorMessage will hold the string error message.
    // if error is 0, do not access errorMessage.
    char* errorMessage;

    // JSON blob containing results
} ApiReturn;

typedef struct DetectorReturn
{

    // 0 means success, non-0 means failures
    int error;

    // if error is non-0, errorMessage will hold the string error message.
    // if error is 0, do not access errorMessage.
    char* errorMessage;

    // if error is non-0, do not access resultsJson
    // if error is 0, results JSON will contain a JSON object with a list of
    // speech_start and speech_end events, with timestamps.
    char* resultsJson;
} DetectorReturn;


typedef struct AudioEvent {
        short* audio;
        int size;
} DetectionEvent;

#endif
