// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.cc
// Mission Supervisor class implementation

#include "mission_supervisor.h"

#include <iostream>

#include <robot_instr.h>

namespace IDP {
    /**
     * Construct the MissionSupervisor.
     * Initialises a link to the specified robot number, or 0 if running
     * embedded.
     * \param robot Which robot to link to, or 0 if embedded
     */
    MissionSupervisor::MissionSupervisor(int robot = 0)
    {
        int status;
        this->rlink = new robot_link;

        // Initialise link
        std::cout << "[MisSup] Initialising" << std::endl;
        if(robot == 0) {
            status = this->rlink->initialise();
        } else {
            status = this->rlink->initialise(robot);
        }

        // Check link
        if(!status) {
            std::cerr << "[MisSup] ERROR: Initialising rlink" << std::endl;
            std::cerr << "[MisSup] Not initialising" << std::endl;
            return;
        }
    }

    /**
     * Set both motors driving forwards.
     */
    void MissionSupervisor::drive_forward()
    {

        // Low ramping
        rlink->command(RAMP_TIME, 64);

        std::cout << "[MisSup] Driving forward" << std::endl;
        rlink->command(MOTOR_1_GO, (0<<7) | 127);
        rlink->command(MOTOR_2_GO, (1<<7) | 127);
    }

    /**
     * Set both motors driving backwards.
     */
    void MissionSupervisor::drive_backward()
    {

        // Low ramping
        rlink->command(RAMP_TIME, 64);

        std::cout << "[MisSup] Driving backwards" << std::endl;
        rlink->command(MOTOR_1_GO, (1<<7) | 127);
        rlink->command(MOTOR_2_GO, (0<<7) | 127);
    }

    /**
     * Stop all motors.
     */
    void MissionSupervisor::stop() {
        std::cout << "[MisSup] Stopping" << std::endl;
        rlink->command(MOTOR_1_GO, 0);
        rlink->command(MOTOR_2_GO, 0);
        rlink->command(MOTOR_3_GO, 0);
        rlink->command(MOTOR_4_GO, 0);
    }
}

