// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// debug.h
// Various debug related functionality defines.
//
// Usage:
// define MODULE_NAME to something relevant to the module, and
// set TRACE_ENABLED, DEBUG_ENABLED, INFO_ENABLED and ERROR_ENABLED to true or
// false as desired, then include this file. Only include in source files
// and as a final include after any other files to prevent other
// sources from overriding defines.

#ifdef MODULE_NAME

#include <iostream>

#if TRACE_ENABLED
#define TRACE(x) std::cout << "[" << MODULE_NAME << "][T] " << x << std::endl;
#else
#define TRACE(x)
#endif

#if DEBUG_ENABLED
#define DEBUG(x) std::cout << "[" << MODULE_NAME << "][D] " << x << std::endl;
#else
#define DEBUG(x)
#endif

#if INFO_ENABLED
#define INFO(x) std::cout << "[" << MODULE_NAME << "][I] " << x << std::endl;
#else
#define INFO(x)
#endif

#if ERROR_ENABLED
#define ERROR(x) std::cout << "[" << MODULE_NAME << "][E] " << x << std::endl;
#else
#define ERROR(x)
#endif

#endif
