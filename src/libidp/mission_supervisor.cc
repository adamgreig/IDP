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

        // Time many iterations of the test instruction
        int i;
        stopwatch sw;
        sw.start();
        for(i=0; i<1024; i++) {
            rlink.request(TEST_INSTRUCTION);
        }
        int time = sw.stop();

        std::cout << "Ran 1024 iterations of TEST_INSTRUCTION:" << std::endl;
        std::cout << "\tTotal time: " << time << "ms" << std::endl;
        std::cout << "\tTime per instruction: " << time / 1024 << "ms";
        std::cout << std::endl;
    }
}

