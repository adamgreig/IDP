// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.cc
// Line Following class implementation

#include "line_following.h"

#include <iostream>

#include <robot_instr.h>

#include "hal.h"

namespace IDP {
    /**
     * Construct the Line Follower
     */
    LineFollowing::LineFollowing(const HardwareAbstractionLayer* hal)
        : _hal(hal)
    {
        std::cout << "[LineFollowing] Initialising a Line Follower";
        std::cout << std::endl;

        // Accumulator
        _error = 0;
        
    }

    /**
     * Call this function many times to follow a line
     */
    void LineFollowing::follow_line() {

        std::cout << "[LineFollowing] Reading IR sensors" << std::endl;

        // Read the state of the IR sensors from hal
        const LineSensors sensors = _hal->line_following_sensors();

        if (sensors.line_left == LINE && sensors.line_right == LINE)
        {
            std::cout << "[LineFollowing] On the line" << std::endl;

            // We are on the line so reset the error and continue
            this->_error = 0;
        }
        else if (sensors.line_left == LINE && sensors.line_right == NO_LINE)
        {
            std::cout << "[LineFollowing] Too far right" << std::endl;

            // Need to turn left
            this->_error--;
        }
        else if (sensors.line_left == NO_LINE && sensors.line_right == LINE)
        {
            std::cout << "[LineFollowing] Too far left" << std::endl;

            // Need to turn right
            this->_error++;
        }
        else if (sensors.line_left == NO_LINE && sensors.line_right == NO_LINE)
        {
            std::cout << "[LineFollowing] Lost the line, stopping";
            std::cout << std::endl;

            // On the line, continue
            // this->_hal->motors_stop();
        }

        // Call correct_steering to carry out any required adjustments
        this->correct_steering(_error);
    }

    /**
     * Correct the steering of the robot
     * \param _error A signed integer where negative is too far left, and
     * positive is too far right
     */
    void LineFollowing::correct_steering(int _error)
    {
        std::cout << "[LineFollowing] Correcting steering with error ";
        std::cout << _error << std::endl;

        short int correction = static_cast<short int>(ki) 
            * static_cast<short int>(_error);

        // Clip correction to the motors' max speed
        if (correction > MOTOR_MAX_SPEED)
            correction = MOTOR_MAX_SPEED;
        if (correction < -MOTOR_MAX_SPEED)
            correction = -MOTOR_MAX_SPEED;

        if (correction < 0)
        {
            // We are too far left, so turn right
            this->_hal->motor_left_go(64);
            this->_hal->motor_right_go(64 - correction);
        }
        else if (correction > 0)
        {
            // We are too far right, so turn left 
            this->_hal->motor_right_go(64);
            this->_hal->motor_left_go(64 + correction);
        }
        else
        {
            // No adjustments are required
            this->_hal->motors_forward(64);
        }
    }
}

