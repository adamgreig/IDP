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

/**
 * Contains all the IDP related functionality including libidp and some idpbin
 * classes.
 */
namespace IDP {

    class HardwareAbstractionLayer;
    enum BobbinColour;
    enum Box;

    /**
     * Control the overall robot behaviour and objective
     * fulfillment
     */
    class MissionSupervisor
    {
        public:
            MissionSupervisor(int robot);
            ~MissionSupervisor();
            void run_task();
            const HardwareAbstractionLayer* hal() const;
        private:
            void update_box_contents(BobbinColour colour);
            BobbinColour find_useful_bobbin();
            HardwareAbstractionLayer* _hal;
            bool _box_has_white;
            bool _box_has_red;
            bool _box_has_green;
    };
}

#endif /* LIBIDP_MISSION_SUPERVISOR_H */

