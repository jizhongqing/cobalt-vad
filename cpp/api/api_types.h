#ifndef COBALT_GUARD_API_TYPES_H_
#define COBALT_GUARD_API_TYPES_H_
/**
 * @file api_types.h
 * types used by the cobalt api.
 */

// Logging Levels, a subset of log4j levels
#define LOGLEVEL_TRACE 0 // most fine grained log level available
#define LOGLEVEL_DEBUG 1 // fine grained messages to aid debugging
#define LOGLEVEL_INFO 2  // coarse grained information messages, mostly event based
#define LOGLEVEL_WARN 3  // warning messages, potentially harmful situations
#define LOGLEVEL_ERROR 4 // error events but application will keep going
#define LOGLEVEL_FATAL 5 // application will abort shortly

#endif
