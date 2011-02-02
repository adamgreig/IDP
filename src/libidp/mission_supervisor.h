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

#include <robot_link.h>

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
            void run_task();
            void drive_forward();
            void drive_backward();
            void stop();
            void test_line_sensor();
            void test_line_following();
            const HardwareAbstractionLayer* hal() const;
        private:
            HardwareAbstractionLayer* _hal;
    };
}

#endif /* LIBIDP_MISSION_SUPERVISOR_H */

