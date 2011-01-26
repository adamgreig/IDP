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

        // Low ramping
        rlink.command(RAMP_TIME, 64);

        for(;;) {
            // Drive forward a bit
            std::cout << "Driving forward a bit..." << std::endl;
            rlink.command(MOTOR_1_GO, (1<<7) | 63);
            rlink.command(MOTOR_2_GO, (1<<7) | 127);
            delay(5000);
            std::cout << "Turning a bit..." << std::endl;
            rlink.command(MOTOR_1_GO, (1<<7) | 63);
            rlink.command(MOTOR_2_GO, (0<<7) | 127);
            delay(3000);
        }

    }
}

