// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// self_tests.cc
// Self Tests class implementation

#include <iostream>
#include <robot_instr.h>

#include "mission_supervisor.h"
#include "hal.h"
#include "line_following.h"

namespace IDP {
    /**
     * Constuct a SelfTest instance
     * Completely seperate to mission supervisor and initialises own
     * link to robot, with its own HAL instance
     * \param robot Which robot to link to, or 0 if embedded
     */
    SelfTests::SelfTests(int robot = 0)
    {
    }

    /**
     * Drive the robot forwards for a moment
     */
    void SelfTests::drive_forward()
    {
    }

    /**
     * Drive the robot backwards for a moment
     */
    void SelfTests::drive_backward()
    {
    }

    /**
     * Stop all of the robot's motors
     */
    void SelfTests::stop()
    {
    }

    /**
     * Drive motors in opposite directions to turn the robot left on
     * the spot
     */
    void SelfTests::turn_left()
    {
    }

    /**
     * Drive motors in opposite directions to turn the robot right on
     * the spot
     */
    void SelfTests::turn_right()
    {
    }
    
    /**
     * Drive forwards for a moment whilst reducing the speed of the 
     * left motor relative to the right to steer left
     */
    void SelfTests::steer_left()
    {
    }

    /**
     * Drive forwards for a moment whilst reducing the speed of the
     * right motor relative to the left to steer right
     */
    void SelfTests::steer_right()
    {
    }

    /**
     * Display the status (LINE or NO_LINE) of each of the four IR
     * line following sensors
     */
    void SelfTests::line_sensors()
    {
    }

    /**
     * Display the state of each of the two microswitches
     */
    void SelfTests::microswitches()
    {
    }
    
    /**
     * Display the current ADC read from the light dependent resistor
     */
    void SelfTests::LDRs()
    {
    }

    /**
     * Fire each of the actuators in turn
     */
    void SelfTests::actuators()
    {
    }

    /**
     * Follow a line until further notice, without caring where we end up
     */
    void SelfTests::line_following()
    {
    }

    /**
     * Use the actuators to pick up an object before placing it back down
     * again
     */
    void SelfTests::clamp_control()
    {
    }

    /**
     * Analyse the colour of the bobbin that is currently being held
     * in the clamp
     */
    void SelfTests::bobbin_analyse()
    {
    }

    /**
     * Select a source and destination and then navigate to the
     * destination assuming we are starting at the source
     */
    void SelfTests::navigate()
    {
    }

    /**
     * Drive slowly looking for an object in range for pickup, 
     * then position self ready to clamp said object
     */
    void SelfTests::position()
    {
    }

    /**
     * Turn on each of the status LEDs (used for indicating bobbin
     * colour) in turn
     */
    void SelfTests::status_LEDs()
    {
    }

    /**
     * Turn on each of the coloured LEDs used for colour detection
     * in turn
     */
    void SelfTests::colour_sensor_LEDs()
    {
    }

    /**
     * Turn on the LED used for detecting bad bobbins
     */
    void SelfTests::badness_LED(void)
    {
    }
}
