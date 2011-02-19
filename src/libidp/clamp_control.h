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
            BobbinColour box_colour() const;
            BobbinBadness badness() const;
            bool bobbin_present();
            bool box_present();
            void open_jaw(void);
            void close_jaw(void);
            void raise_arm(void);
            void lower_arm(void);
            unsigned short int average_bad_ldr(unsigned short int n = 3)
                const;
            unsigned short int average_colour_ldr(unsigned short int n = 3)
                const;
        private:
            HardwareAbstractionLayer* _hal;
            short int _red_box_level;
            short int _green_box_level;
            short int _red_rack_level;
            short int _green_rack_level;
            short int _box_present_level;
            short int _white_present_level;
            short int _coloured_present_level;
            short int _badness_light_zero;
            short int _badness_dark_zero;
            short int _colour_light_zero;
            short int _colour_light_closed_zero;
            short int _colour_light_box_zero;
            bool _arm_up;
            bool _jaw_open;
    };
}

#endif /* LIBIDP_CLAMP_CONTROL_H */

