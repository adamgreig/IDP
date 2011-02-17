// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.h
// Mission Supervisor class definition
//
// Mission Supervisor - control overall robot behaviour and objective
// fulfillment

#pragma once
#ifndef LIBIDP_MISSION_SUPERVISOR_H
#define LIBIDP_MISSION_SUPERVISOR_H

// Required for their various enums
#include "clamp_control.h"
#include "navigation.h"

/**
 * Contains all the IDP related functionality including libidp and some idpbin
 * classes.
 */
namespace IDP {

    class HardwareAbstractionLayer;

    /**
     * Control the overall robot behaviour and objective
     * fulfillment
     */
    class MissionSupervisor
    {
        public:
            MissionSupervisor(int robot);
            ~MissionSupervisor();
            void run_task(void);
            void stop(void);
            const HardwareAbstractionLayer* hal() const;
        private:
            void update_box_contents(BobbinColour colour);
            BobbinColour find_useful_bobbin(void);
            void fill_and_deliver(Box box);
            HardwareAbstractionLayer* _hal;
            Navigation* _nav;
            ClampControl* _cc;
            bool _box_has_red;
            bool _box_has_green;
            bool _box_has_white;
    };
}

#endif /* LIBIDP_MISSION_SUPERVISOR_H */

