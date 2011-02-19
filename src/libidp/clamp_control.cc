// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.cc
// Clamp Control class implementation

// Use unistd.h for sleep functionality
#include <unistd.h>

// abs()
#include <cstdlib>

#include <fstream>

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
    _arm_up(true), _jaw_open(true)
    {
        TRACE("ClampControl("<<hal<<")");
        INFO("Initialising a ClampControl");

        INFO("Reading levels from file...");
        std::ifstream f("levelsfile");
        f >> _colour_light_closed_zero >> _colour_light_zero
            >> _badness_light_zero >> _badness_dark_zero
            >> _colour_light_box_zero
            >> _red_box_level >> _green_box_level >> _red_rack_level
            >> _green_rack_level >> _box_present_level >> _white_present_level
            >> _coloured_present_level;
        f.close();

        this->open_jaw();
        this->raise_arm();
    }

    /**
     * Pick up something using the clamp.
     */
    void ClampControl::pick_up()
    {
        TRACE("pick_up()");
        INFO("Picking something up");
        
        DEBUG("Opening grabber jaw");
        this->open_jaw();

        DEBUG("Lowering grabber arm");
        this->lower_arm();

        DEBUG("Closing grabber jaw");
        this->close_jaw();

        DEBUG("Raising grabber arm");
        this->raise_arm();
    }

    /**
     * Put something in the clamp down
     */
    void ClampControl::put_down()
    {
        TRACE("put_down()");
        INFO("Putting something down");

        DEBUG("Lowering grabber arm");
        this->lower_arm();

        DEBUG("Opening grabber jaw");
        this->open_jaw();

        // Make sure we return with the grabber lifted
        DEBUG("Raising grabber arm");
        this->raise_arm();
    }

    /**
     * Raise the grabber arm.
     */
    void ClampControl::raise_arm()
    {
        TRACE("raise_arm()");
        //if(!this->_arm_up) {
            DEBUG("Lifting the grabber");
            this->_hal->grabber_lift(true);
            usleep(1500000);
            this->_arm_up = true;
        //} else {
            //DEBUG("Asked to raise the arm but it was already raised");
        //}
    }

    /**
     * Lower the grabber arm.
     */
    void ClampControl::lower_arm()
    {
        TRACE("lower_arm()");
        //if(this->_arm_up) {
            DEBUG("Lowering the grabber");
            this->_hal->grabber_lift(false);
            usleep(2000000);
            this->_arm_up = false;
        //} else {
            //DEBUG("Asked to lower the grabber but it was already lowered");
        //}
    }

    /**
     * Open the grabber jaw.
     */
    void ClampControl::open_jaw()
    {
        TRACE("open_jaw()");
        //if(!this->_jaw_open) {
            DEBUG("Releasing the grabber jaw");
            this->_hal->grabber_jaw(false);
            usleep(1000000);
            this->_jaw_open = true;
        //} else {
            //DEBUG("Asked to open the jaw but it was already open");
        //}
    }

    /**
     * Close the grabber jaw.
     */
    void ClampControl::close_jaw()
    {
        TRACE("open_jaw()");
        //if(this->_jaw_open) {
            DEBUG("Clamping the grabber jaw");
            this->_hal->grabber_jaw(true);
            usleep(1000000);
            this->_jaw_open = false;
        //} else {
            //DEBUG("Asked to close the jaw but it was already closed");
        //}
    }

    /**
     * Check the bobbin colour
     * \return A BobbinColour value to indicate current bobbin colour
     */
    BobbinColour ClampControl::colour() const
    {
        TRACE("colour()");
        INFO("Checking bobbin colour");
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(false);
        unsigned short int reading = this->average_colour_ldr();
        DEBUG("Read " << reading);
        this->_hal->colour_LED(false);

        short int delta = reading - this->_colour_light_closed_zero;
        DEBUG("Delta " << delta);

        if(delta < _red_rack_level)
        {
            DEBUG("Found a red bobbin");
            this->_hal->indication_LEDs(false, false, true);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_RED;
        }
        else if(delta < _green_rack_level)
        {
            DEBUG("Found a green bobbin");
            this->_hal->indication_LEDs(false, true, false);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_GREEN;
        }
        else
        {
            DEBUG("Found a white bobbin");
            this->_hal->indication_LEDs(true, false, false);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_WHITE;
        }
    }

    /**
     * Check the bobbin colour while it is in a box
     */
    BobbinColour ClampControl::box_colour() const
    {
        TRACE("box_colour()");
        INFO("Checking box colour");
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(false);
        unsigned short int reading = this->average_colour_ldr();
        DEBUG("Read " << reading);
        this->_hal->colour_LED(false);

        short int delta = reading - this->_colour_light_box_zero;
        DEBUG("Delta " << delta);

        if(delta < _red_box_level)
        {
            DEBUG("Found a red bobbin in a box");
            this->_hal->indication_LEDs(false, false, true);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_RED;
        }
        else if(delta < _green_box_level)
        {
            DEBUG("Found a green bobbin in a box");
            this->_hal->indication_LEDs(false, true, false);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_GREEN;
        }
        else
        {
            DEBUG("Found a white bobbin in a box");
            this->_hal->indication_LEDs(true, false, false);
            usleep(500000);
            this->_hal->indication_LEDs(true, true, true);
            return BOBBIN_WHITE;
        }
    }

    /**
     * Check the bobbin badness
     * \return A BobbinBadness value to indicate current bobbin status
     */
    BobbinBadness ClampControl::badness() const
    {
        TRACE("badness()");
        INFO("Checking for bobbin badness");
        this->_hal->bad_bobbin_LED(false);
        short unsigned int reading = this->average_bad_ldr();
        DEBUG("Got a badness LDR value of " << reading);
        //unsigned int delta = reading - this->_badness_light_zero;
        //if(reading > _bad_level - _badness_tolerance
                //&& reading < _bad_level + _badness_tolerance)
        return BOBBIN_GOOD;
        //if(delta > 110)
        //{
            //INFO("Found a bad bobbin");
            //this->_hal->bad_bobbin_LED(false);
            //return BOBBIN_BAD;
        //}
        //else
        //{
            //INFO("Found a good bobbin");
            //this->_hal->bad_bobbin_LED(false);
            //return BOBBIN_GOOD;
        //}
    }

    /**
     * See if a bobbin is in the jaw.
     * 
     * Used especially when navigating down the rack to check when we've found
     * something.
     * The red and green bobbins block a lot of reflected light from the metal
     * backplate, so use this large drop in light to notice them. The white
     * bobbins reflect about the same amount, but when present without lights
     * on they will block a good deal of light from hitting the sensor anyway,
     * so we can detect them as such.
     * \returns True when a bobbin is found
     */
    bool ClampControl::bobbin_present()
    {
        TRACE("bobbin_present()");
        DEBUG("Checking for bobbins..."); 

        // Turn on the light
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(false);

        // Get a reading in the light (for red and green)
        unsigned short int reading = this->average_colour_ldr();
        short int delta = reading - this->_colour_light_zero;
        
        // Turn off the light
        this->_hal->colour_LED(false);

        DEBUG("Light: Read " << reading << ", delta " << delta);

        // If the light reading indicates that we found a bobbin, we can return
        if(delta < _coloured_present_level)
            return true;

        // Discard readings while it settles
        this->average_bad_ldr(10);

        // Take a reading with lights off (for white)
        reading = this->average_bad_ldr();
        delta = reading - this->_badness_dark_zero;

        DEBUG("Dark: Read " << reading << ", delta " << delta);

        return (delta > _white_present_level);
    }

    /**
     * See if a box is under the jaw.
     *
     * We do this by checking for reflections of the steel box under the bad
     * bobbin sensor.
     */
    bool ClampControl::box_present()
    {
        TRACE("box_present()");
        DEBUG("Checking for a box...");
        
        // Turn on the light
        this->_hal->bad_bobbin_LED(true);
        this->_hal->colour_LED(false);

        // Read the LDR
        unsigned short int reading = this->average_bad_ldr();
        short int delta = reading - this->_badness_light_zero;

        // Turn off the light
        this->_hal->bad_bobbin_LED(false);

        DEBUG("Reading " << reading << ", delta " << delta);

        return (delta > _box_present_level);
    }

    /**
     * Take an average bad bobbin LDR reading of n samples.
     * \param n Number of samples to take, default 3
     */
    unsigned short int ClampControl::average_bad_ldr(unsigned short int n)
        const
    {
        TRACE("average_bad_ldr(" << n << ")");
        DEBUG("Taking a badness LDR average reading");
        unsigned int total = 0;
        unsigned short int i;
        for(i = 0; i < n; i++)
            total += this->_hal->bad_bobbin_ldr();
        return static_cast<unsigned short int>(total / n);
    }

    /**
     * Take an average colour LDR reading of n samples.
     * \param n Number of samples to take, default 3
     */
    unsigned short int ClampControl::average_colour_ldr(unsigned short int n)
        const
    {
        TRACE("average_colour_ldr(" << n << ")");
        DEBUG("Taking a colour LDR average reading");
        unsigned int total = 0;
        unsigned short int i;
        for(i = 0; i < n; i++)
            total += this->_hal->colour_ldr();
        return static_cast<unsigned short int>(total / n);
    }

}

