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

    class HardwareAbstrationLayer;

    /**
     * Bobbin colours
     */
    enum BobbinColour {
        BOBBIN_RED,
        BOBBIN_GREEN,
        BOBBIN_WHITE
    };

    /**
     * Bobbin good or bad
     */
    enum BobbinBadness {
        BOBBIN_GOOD,
        BOBBIN_BAD
    };
    
    class ClampControl
    {
        public:
            ClampControl(const HardwareAbstractionLayer* hal);
            void pick_up();
            void put_down();
            const BobbinColour colour() const;
            const BobbinBadness badness() const;
    };
}

#endif /* LIBIDP_CLAMP_CONTROL_H */

