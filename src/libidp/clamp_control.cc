// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.cc
// Clamp Control class implementation

#include "clamp_control.h"

// Debug functionality
#define MODULE_NAME "Clamp"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {

    /**
     * String representation of BobbinColour
     */
    const char* BobbinColourStrings[] = {
        "BOBBIN_RED",
        "BOBBIN_GREEN",
        "BOBBIN_WHITE"
    };

    /**
     * String representation of BobbinBadness
     */
    const char* BobbinBadnessStrings[] = {
        "BOBBIN_GOOD",
        "BOBBIN_BAD"
    };

    /**
     * Initialise the class, storing the const pointer to the HAL.
     * \param hal A const pointer to an instance of the HAL
     */
    ClampControl::ClampControl(HardwareAbstractionLayer* hal): _hal(hal)
    {
        TRACE("ClampControl("<<hal<<")");
        INFO("Initialising a ClampControl");
    }

    /**
     * Pick up something using the clamp.
     */
    void ClampControl::pick_up()
    {
        TRACE("pick_up()");
    }

    /**
     * Put something in the clamp down
     */
    void ClampControl::put_down()
    {
        TRACE("put_down()");
    }

    /**
     * Check the bobbin colour
     * \return A BobbinColour value to indicate current bobbin colour
     */
    BobbinColour ClampControl::colour() const
    {
        TRACE("colour()");
        return BOBBIN_WHITE;
    }

    /**
     * Check the bobbin badness
     * \return A BobbinBadness value to indicate current bobbin status
     */
    BobbinBadness ClampControl::badness() const
    {
        TRACE("badness()");
        return BOBBIN_BAD;
    }

}

