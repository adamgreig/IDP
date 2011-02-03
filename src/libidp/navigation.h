// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.h
// Navigation class definition
//
// Navigation - Routefinding and position estimation control.

#pragma once
#ifndef LIBIDP_NAVIGATION_H
#define LIBIDP_NAVIGATION_H

namespace IDP {
    
    class HardwareAbstractionLayer;

    /**
     * Current navigation status
     */
    enum NavigationStatus {
        NAVIGATION_ENROUTE,
        NAVIGATION_ARRIVED,
        NAVIGATION_LOST
    };

    /**
     * Navigation's current position estimate
     */
    enum NavigationLocation {
        NAVIGATION_BOXES,
        NAVIGATION_RACK,
        NAVIGATION_DELIVERY
    };

    /**
     * Find a route from one place to another on the board, and
     * maintain an estimate of the current position
     */
    class Navigation
    {
        public:
            Navigation(const HardwareAbstractionLayer* hal);
            const NavigationStatus go(const NavigationLocation location);
        private:
            const HardwareAbstractionLayer* _hal;
    };
}

#endif /* LIBIDP_NAVIGATION_H */

