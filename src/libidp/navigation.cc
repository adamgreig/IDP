// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.cc
// Navigation class implementation

#include "navigation.h"

#include <iostream>

namespace IDP {

    /**
     * Initialise the class, storing the const pointer to the HAL.
     * \param hal A const pointer to an instance of the HAL
     */
    Navigation::Navigation(const HardwareAbstractionLayer* hal): _hal(hal)
    {
    }

    /**
     * Go to a location.
     * \returns A navigation status code
     */
    const NavigationStatus Navigation::go(const NavigationLocation location)
    {
    }
}

