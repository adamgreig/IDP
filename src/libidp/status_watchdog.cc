// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// status_watchdog.cc
// Status Watchdog class implementation
//

#include "status_watchdog.h"

// Debug functionality
#define MODULE_NAME "StatusWatch"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {
    /**
     * Read the STATUS register of the microcontroller and return the value
     * \returns The error encountered, if any 
     */
    int StatusWatchdog::check() const {
        TRACE("check()");
        return 0;
    }
}
