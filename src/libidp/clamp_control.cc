// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.cc
// Clamp Control class implementation

// Use unistd.h for sleep functionality
#include <unistd.h>

#include "clamp_control.h"
#include "hal.h"

// Debug functionality
#define MODULE_NAME "Clamp"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {

    /**
     * Initialise the class, storing the const pointer to the HAL.
     * \param hal A const pointer to an instance of the HAL
     */
    ClampControl::ClampControl(HardwareAbstractionLayer* hal): _hal(hal),
    _colour_tolerance(5), _badness_tolerance(10), _red_level(165),
    _green_level(144),_white_level(191),_bad_level(203)
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
        this->_hal->grabber_jaw(false);
        usleep(1E6);
        this->_hal->grabber_lift(false);
        usleep(1E6);
        this->_hal->grabber_jaw(true);
        usleep(1E6);
        this->_hal->grabber_lift(true);
        usleep(1E6);
    }

    /**
     * Put something in the clamp down
     */
    void ClampControl::put_down()
    {
        TRACE("put_down()");
        this->_hal->grabber_lift(false);
        usleep(1E6);
        this->_hal->grabber_jaw(false);
        usleep(1E6);
    }

    /**
     * Check the bobbin colour
     * \return A BobbinColour value to indicate current bobbin colour
     */
    BobbinColour ClampControl::colour() const
    {
        TRACE("colour()");
        this->_hal->colour_LEDs(true, true);
        unsigned short int reading = this->_hal->colour_ldr();
        DEBUG("Got an ADC read of " << reading);
        if(reading > _red_level - _colour_tolerance
                && reading < _red_level + _colour_tolerance)
        {
            DEBUG("Found a red bobbin");
            this->_hal->colour_LEDs(false, false);
            return BOBBIN_RED;
        }
        else if(reading > _green_level - _colour_tolerance
                && reading < _green_level + _colour_tolerance)
        {
            DEBUG("Found a green bobbin");
            this->_hal->colour_LEDs(false, false);
            return BOBBIN_GREEN;
        }
        else if(reading > _white_level - _colour_tolerance
                && reading < _white_level + _colour_tolerance)
        {
            DEBUG("Found a white bobbin");
            this->_hal->colour_LEDs(false, false);
            return BOBBIN_WHITE;
        }
        else
        {
            // No idea what colour this bobbin is
            ERROR("Couldn't identify bobbin colour");
            this->_hal->colour_LEDs(false, false);
            return BOBBIN_UNKNOWN_COLOUR;
        }
    }

    /**
     * Check the bobbin badness
     * \return A BobbinBadness value to indicate current bobbin status
     */
    BobbinBadness ClampControl::badness() const
    {
        TRACE("badness()");
        this->_hal->bad_bobbin_LED(true);
        short unsigned int reading = this->_hal->bad_bobbin_ldr();
        DEBUG("Got a badness LDR value of " << reading);
        if(reading > _bad_level - _badness_tolerance
                && reading < _bad_level + _badness_tolerance)
        {
            DEBUG("Found a bad bobbin");
            this->_hal->bad_bobbin_LED(false);
            return BOBBIN_BAD;
        }
        else
        {
            DEBUG("Found a good bobbin");
            this->_hal->bad_bobbin_LED(false);
            return BOBBIN_GOOD;
        }
    }

    void ClampControl::calibrate(unsigned short int red, 
            unsigned short int green, unsigned short int white)
    {
        this->_red_level = red;
        this->_green_level = green;
        this->_white_level = white;
    }

}

