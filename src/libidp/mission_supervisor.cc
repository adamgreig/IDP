// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.cc
// Mission Supervisor class implementation

#include "mission_supervisor.h"

#include <iostream>
#include <robot_instr.h>
#include <robot_link.h>

namespace IDP {
    MissionSupervisor::MissionSupervisor()
    {
        std::cout << "Initialising a MissionSupervisor..." << std::endl;
        robot_link rlink;
        if(!rlink.initialise(39)) {
            std::cout << "Error initialising rlink" << std::endl;
        }
        int val = rlink.request(TEST_INSTRUCTION);
        std::cout << "Finished test request, val = " << val << std::endl;

        rlink.print_errs("[Error] ");
    }
}

