// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.cc
// Mission Supervisor class implementation

#include "mission_supervisor.h"

#include <iostream>

#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
#include <delay.h>

namespace IDP {
    MissionSupervisor::MissionSupervisor()
    {
        // Initialise link
        std::cout << "Initialising a MissionSupervisor..." << std::endl;
        robot_link rlink;
        if(!rlink.initialise(39)) {
            std::cout << "Error initialising rlink" << std::endl;
            return;
        }

        // Clear the status register
        std::cout << "Clearing the status register..." << std::endl;
        rlink.request(STATUS);

        // Spin motor one
        std::cout << "Spinning up motor one..." << std::endl;
        rlink.command(RAMP_TIME, 0);
        rlink.command(MOTOR_3_GO, 127);

        for(;;);
    }
}

