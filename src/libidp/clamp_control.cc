// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.cc
// Clamp Control class implementation

#include "clamp_control.h"

#include <iostream>

namespace IDP {

    /**
     * Initialise the class, storing the const pointer to the HAL.
     * \param hal A const pointer to an instance of the HAL
     */
    ClampControl::ClampControl(const HardwareAbstractionLayer* hal): _hal(hal)
    {
    }

    /**
     * Pick up something using the clamp.
     */
    void ClampControl::pick_up()
    {
    }

    /**
     * Put something in the clamp down
     */
    void ClampControl::put_down()
    {
    }

    /**
     * Check the bobbin colour
     * \return A BobbinColour value to indicate current bobbin colour
     */
    const BobbinColour ClampControl::colour() const
    {
    }

    /**
     * Check the bobbin badness
     * \return A BobbinBadness value to indicate current bobbin status
     */
    const BobbinBadness ClampControl::badness() const
    {
    }

}

