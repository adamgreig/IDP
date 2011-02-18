// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.cc
// Clamp Control class implementation

// Use unistd.h for sleep functionality
#include <unistd.h>

// abs()
#include <cstdlib>

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
    _green_level(144), _white_level(191), _bad_level(203),
    _badness_light_zero(0), _badness_dark_zero(0), _colour_light_zero(0),
    _colour_dark_zero(0)
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
        DEBUG("Lifting the grabber");
        this->_hal->grabber_lift(true);
        usleep(1500000);
    }

    /**
     * Lower the grabber arm.
     */
    void ClampControl::lower_arm()
    {
        TRACE("lower_arm()");
        DEBUG("Lowering the grabber");
        this->_hal->grabber_lift(false);
        usleep(2000000);
    }

    /**
     * Open the grabber jaw.
     */
    void ClampControl::open_jaw()
    {
        TRACE("open_jaw()");
        DEBUG("Releasing the grabber jaw");
        this->_hal->grabber_jaw(false);
        usleep(1000000);
    }

    /**
     * Close the grabber jaw.
     */
    void ClampControl::close_jaw()
    {
        TRACE("open_jaw()");
        DEBUG("Clamping the grabber jaw");
        this->_hal->grabber_jaw(true);
        usleep(1000000);
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
        unsigned short int reading = this->average_colour_ldr();
        DEBUG("Read " << reading);
        this->_hal->colour_LED(false);

        short int delta = reading - this->_colour_light_zero;
        DEBUG("Delta " << delta);

        if(delta < -80)
            return BOBBIN_RED;
        else if(delta < -60)
            return BOBBIN_GREEN;
        else
            return BOBBIN_WHITE;
    }

    /**
     * Check the bobbin colour while it is in a box
     */
    BobbinColour ClampControl::box_colour() const
    {
        TRACE("box_colour()");
        INFO("Checking box colour");
        this->_hal->colour_LED(true);
        unsigned short int reading = this->average_colour_ldr();
        DEBUG("Read " << reading);
        this->_hal->colour_LED(false);

        short int delta = reading - this->_colour_light_zero;
        DEBUG("Delta " << delta);

        if(delta < -10)
            return BOBBIN_RED;
        else if(delta < -3)
            return BOBBIN_GREEN;
        else
            return BOBBIN_WHITE;
    }

    /**
     * Check the bobbin badness
     * \return A BobbinBadness value to indicate current bobbin status
     */
    BobbinBadness ClampControl::badness() const
    {
        TRACE("badness()");
        INFO("Checking for bobbin badness");
        this->_hal->bad_bobbin_LED(true);
        short unsigned int reading = this->average_bad_ldr();
        DEBUG("Got a badness LDR value of " << reading);
        if(reading > _bad_level - _badness_tolerance
                && reading < _bad_level + _badness_tolerance)
        {
            INFO("Found a bad bobbin");
            this->_hal->bad_bobbin_LED(false);
            return BOBBIN_BAD;
        }
        else
        {
            INFO("Found a good bobbin");
            this->_hal->bad_bobbin_LED(false);
            return BOBBIN_GOOD;
        }
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

        // If we haven't read this before, store the value. Next time
        // we'll find a delta.
        if(_colour_light_zero == 0 || _colour_dark_zero == 0) {
            ERROR("No previous baseline was set, reading one now instead.");
            this->store_zero();
        }

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
        if(delta < -7)
            return true;

        // Discard readings while it settles
        this->average_bad_ldr(10);

        // Take a reading with lights off (for white)
        reading = this->average_bad_ldr();
        delta = reading - this->_badness_dark_zero;

        DEBUG("Dark: Read " << reading << ", delta " << delta);

        return (delta > 15);
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
        
        // If we haven't read this before, store the value. Next time
        // we'll find a delta.
        if(_badness_light_zero == 0) {
            ERROR("No previous baseline was set, reading one now instead.");
            this->store_zero();
        }

        // Turn on the light
        this->_hal->bad_bobbin_LED(true);
        this->_hal->colour_LED(false);

        // Read the LDR
        unsigned short int reading = this->average_bad_ldr();
        short int delta = reading - this->_badness_light_zero;

        // Turn off the light
        this->_hal->bad_bobbin_LED(false);

        DEBUG("Reading " << reading << ", delta " << delta);

        return (std::abs(delta) > BOX_DETECTION_DELTA_THRESHOLD);
    }


    /**
     * Take an LDR reading and use that as the no-bobbin baseline.
     */
    void ClampControl::store_zero()
    {
        TRACE("store_zero()");
        DEBUG("Taking zero-level readings");

        // Take the two dark readings
        this->_hal->colour_LED(false);
        this->_hal->bad_bobbin_LED(false);
        this->_colour_dark_zero = this->average_colour_ldr(10);
        this->_badness_dark_zero = this->average_bad_ldr(10);

        // Turn on the colour light
        this->_hal->colour_LED(true);
        this->_colour_light_zero = this->average_colour_ldr(10);
        this->_hal->colour_LED(false);

        // Turn on the badness light
        this->_hal->bad_bobbin_LED(true);
        this->_badness_light_zero = this->average_bad_ldr(10);
        this->_hal->bad_bobbin_LED(false);

        INFO("Colour LDR dark level: " << this->_colour_dark_zero);
        INFO("Colour LDR light level: " << this->_colour_light_zero);
        INFO("Badness LDR dark level: " << this->_badness_dark_zero);
        INFO("Badness LDR light level: " << this->_badness_light_zero);
    }

    /**
     * Set the three levels used for bobbin colour detection.
     * \param red The red level (0 to 255)
     * \param green The green level (0 to 255)
     * \param white The white level (0 to 255)
     */
    void ClampControl::calibrate(unsigned short int red, 
            unsigned short int green, unsigned short int white)
    {
        TRACE("calibrate(" << red << ", " << green << ", " << white << ")");
        DEBUG("Setting bobbin colour levels to red:" << red << ", green:" <<
            green << ", white:" << white << ".");

        this->_red_level = red;
        this->_green_level = green;
        this->_white_level = white;
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

