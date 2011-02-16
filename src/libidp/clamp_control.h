// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// clamp_control.h
// Clamp Control class definition
//
// Clamp control - manage actuating the clamp, detecting and analysing
// bobbins for colour and badness, etc

#pragma once
#ifndef LIBIDP_CLAMP_CONTROL_H
#define LIBIDP_CLAMP_CONTROL_H

namespace IDP {

    class HardwareAbstractionLayer;

    /**
     * Bobbin colours
     */
    enum BobbinColour {
        BOBBIN_RED,
        BOBBIN_GREEN,
        BOBBIN_WHITE,
        BOBBIN_UNKNOWN_COLOUR
    };

    /**
     * Bobbin good or bad
     */
    enum BobbinBadness {
        BOBBIN_GOOD,
        BOBBIN_BAD
    };
    
    /**
     * String representation of BobbinColour
     */
    static const char* const BobbinColourStrings[] = {
        "BOBBIN_RED",
        "BOBBIN_GREEN",
        "BOBBIN_WHITE",
        "BOBBIN_UNKNOWN_COLOUR"
    };

    /**
     * String representation of BobbinBadness
     */
    static const char* const BobbinBadnessStrings[] = {
        "BOBBIN_GOOD",
        "BOBBIN_BAD"
    };

    /**
     * Manage the actuation of the clamp, as well as the detection
     * and analysis of bobbins for their colour and badness
     */
    class ClampControl
    {
        public:
            ClampControl(HardwareAbstractionLayer* hal);
            void pick_up();
            void put_down();
            BobbinColour colour() const;
            BobbinBadness badness() const;
            void calibrate(unsigned short int red, unsigned short int green,
                    unsigned short int white);
        private:
            HardwareAbstractionLayer* _hal;
            const unsigned short int _colour_tolerance;
            const unsigned short int _badness_tolerance;
            unsigned short int _red_level;
            unsigned short int _green_level;
            unsigned short int _white_level;
            unsigned short int _bad_level;
    };
}

#endif /* LIBIDP_CLAMP_CONTROL_H */

