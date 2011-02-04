// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.cc
// Mission Supervisor class implementation

#include <iostream>
#include <robot_instr.h>

#include "mission_supervisor.h"
#include "hal.h"
#include "line_following.h"

namespace IDP {
    /**
     * Construct the MissionSupervisor.
     * Initialises a link to the specified robot number, or 0 if running
     * embedded.
     * \param robot Which robot to link to, or 0 if embedded
     */
    MissionSupervisor::MissionSupervisor(int robot = 0)
    {
        // Construct the hardware abstraction layer
        this->_hal = new HardwareAbstractionLayer(robot);
    }

    /**
     * Commence running the main task
     */
    void MissionSupervisor::run_task()
    {
    }

    /**
     * Set both motors driving forwards.
     */
    void MissionSupervisor::drive_forward()
    {
        std::cout << "[MisSup] Driving forward" << std::endl;
        this->_hal->motor_left_forward(127);
        this->_hal->motor_right_forward(127);
    }

    /**
     * Set both motors driving backwards.
     */
    void MissionSupervisor::drive_backward()
    {
        std::cout << "[MisSup] Driving backward" << std::endl;
        this->_hal->motors_backward(127);
    }

    /**
     * Stop all motors.
     */
    void MissionSupervisor::stop()
    {
        std::cout << "[MisSup] Stopping" << std::endl;
        this->_hal->motors_stop();
    }

    /**
     * Attempt to read the line sensor status
     */
    void MissionSupervisor::test_line_sensor()
    {
        std::cout << "[MisSup] Testing line sensor" << std::endl;
        for(;;) {
            this->_hal->clear_status_register();
            LineSensors sensors = this->_hal->line_following_sensors();
            std::cout << "[MisSup] Sensors: ";
            
            if(sensors.outer_left == LINE)
                std::cout << "line, ";
            else
                std::cout << "no line, ";

            if(sensors.line_left == LINE)
                std::cout << "line, ";
            else
                std::cout << "no line, ";

            if(sensors.line_right == LINE)
                std::cout << "line, ";
            else
                std::cout << "no line, ";

            if(sensors.outer_right == LINE)
                std::cout << "line             \r";
            else
                std::cout << "no line          \r";
        }
    }

    /**
     * Test line following on a straight line
     */
    void MissionSupervisor::test_line_following()
    {
        std::cout << "[MisSup] Testing line following" << std::endl;
        LineFollowing lf(this->hal());
        LineFollowingStatus status;
        
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        return;

    }

    /**
     * Const accessor for the HAL
     */
    const HardwareAbstractionLayer* MissionSupervisor::hal() const
    {
        return static_cast<const HardwareAbstractionLayer*> (this->_hal);
    }
}

