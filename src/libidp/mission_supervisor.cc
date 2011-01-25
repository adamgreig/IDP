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

        // Set emergency stop to pin 0 of chip 0
        rlink.command(WRITE_PORT_0, 0xFF);
        rlink.command(STOP_IF_HIGH, (1<<0));
        rlink.command(STOP_SELECT, 0);

        // Turn some motors on
        rlink.command(RAMP_TIME, 0);
        rlink.command(MOTOR_1_GO, 127);

        // Wait for stop
        for(;;) {
            // Read status
            int status = rlink.request(STATUS);
            if(status & (1<<2)) {
                std::cout << "Emergency stop triggered!" << std::endl;
                break;
            }
        }
    }
}

