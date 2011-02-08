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
#include "navigation.h"

// Debug functionality
#define MODULE_NAME "MisSup"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {
    /**
     * Construct the MissionSupervisor.
     * Initialises a link to the specified robot number, or 0 if running
     * embedded.
     * \param robot Which robot to link to, or 0 if embedded
     */
    MissionSupervisor::MissionSupervisor(int robot = 0)
    {
        TRACE("MissionSupervisor(" << robot << ")");
        INFO("Constructing a MisionSupervisor, robot=" << robot);

        // Construct the hardware abstraction layer
        this->_hal = new HardwareAbstractionLayer(robot);
    }

    /**
     * Destruct the MissionSupervisor, deleting the HAL
     */
    MissionSupervisor::~MissionSupervisor()
    {
        TRACE("~MissionSupervisor()");
        delete this->_hal;
    }

    /**
     * Commence running the main task
     */
    void MissionSupervisor::run_task()
    {
        TRACE("run_task()");
    }

    /**
     * Set both motors driving forwards.
     */
    void MissionSupervisor::drive_forward()
    {
        TRACE("drive_forward()");
        INFO("Driving forwards");
        this->_hal->motor_left_forward(127);
        this->_hal->motor_right_forward(127);
    }

    /**
     * Set both motors driving backwards.
     */
    void MissionSupervisor::drive_backward()
    {
        TRACE("drive_backward()");
        INFO("Driving backwards");
        this->_hal->motors_backward(127);
    }

    /**
     * Stop all motors.
     */
    void MissionSupervisor::stop()
    {
        TRACE("stop()");
        INFO("Stopping all motors");
        this->_hal->motors_stop();
    }

    /**
     * Attempt to read the line sensor status
     */
    void MissionSupervisor::test_line_sensor()
    {
        TRACE("test_line_sensor()");
        INFO("Testing line sensors");

        for(;;) {
            this->_hal->clear_status_register();
            LineSensors sensors = this->_hal->line_following_sensors();
            std::cout << "Sensors: ";
            
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
        TRACE("test_line_following()");
        INFO("Testing line following");

        LineFollowing lf(this->_hal);
        LineFollowingStatus status;

        lf.set_speed(127);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("driving over a junction");
        do {
            status = lf.follow_line();
        } while(status == BOTH_TURNS_FOUND);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("driving over a junction");
        do {
            status = lf.follow_line();
        } while(status == BOTH_TURNS_FOUND);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("turning right");
        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("driving over a junction");
        do {
            status = lf.follow_line();
        } while(status == BOTH_TURNS_FOUND);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("turning right");
        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        INFO("turning right");
        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("driving over a junction");
        do {
            status = lf.follow_line();
        } while(status == BOTH_TURNS_FOUND);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);
        
        INFO("driving over a junction");
        do {
            status = lf.follow_line();
        } while(status == BOTH_TURNS_FOUND);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        INFO("turning right");
        do {
            status = lf.turn_right();
        } while(status == ACTION_IN_PROGRESS);

        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);

        this->_hal->motors_stop();

        return;

    }

    /**
     * Test navigation code
     */
    void MissionSupervisor::test_navigation()
    {
        TRACE("test_navigation()");
        INFO("Testing navigation");

        NavigationNode from     = NODE1;
        NavigationNode to       = NODE2;
        NavigationNode target   = NODE11;

        Navigation nav(this->_hal, from, to);
        NavigationStatus status;
        do {
            status = nav.go_node(target);
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();
        return;
    }

    /**
     * Const accessor for the HAL
     */
    const HardwareAbstractionLayer* MissionSupervisor::hal() const
    {
        TRACE("hal()");
        return static_cast<const HardwareAbstractionLayer*> (this->_hal);
    }
}

