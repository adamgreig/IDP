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

        // Clear the I²C error bit
        std::cout << "Clearing the I²C error bit..." << std::endl;
        rlink.request(STATUS);

        // Sequence some LEDs
        std::cout << "Party time!" << std::endl;
        int i, j;
        for(j = 0; j < 5; j++) {
            for(i = 0; i < 8; i++) {
                rlink.command(WRITE_PORT_0, 1<<i);
                delay(50);
            }
            for(i = 7; i >= 0; i--) {
                rlink.command(WRITE_PORT_0, 1<<i);
                delay(50);
            }
        }
    }
}

