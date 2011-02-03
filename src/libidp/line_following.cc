// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.cc
// Line Following class implementation

#include <iostream>
#include <robot_instr.h>

#include "hal.h"
#include "line_following.h"

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
     * Follow a line forwards, stopping when a junction is reached
     */
    void LineFollowing::follow_line() {

        std::cout << "[LineFollowing] Reading IR sensors" << std::endl;

        // Read the state of the IR sensors from hal
        const LineSensors sensors = _hal->line_following_sensors();

        if (sensors.line_left == LINE && sensors.line_right == LINE &&
            sensors.outer_left == NO_LINE && sensors.outer_right == NO_LINE)
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

            // this->_hal->motors_stop();
        }

        if(sensors.outer_right == LINE) {
            std::cout << "[LineFollowing] Found a right turn" << std::endl;
            this->turn_right();
            return;
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
        
        short int max_correction = MOTOR_MAX_SPEED - 64;

        // Clip correction to the motors' max speed
        if (correction > max_correction)
            correction = max_correction;
        if (correction < -max_correction)
            correction = -max_correction;

        if (correction < 0)
        {
            // We are too far left, so turn right
            this->_hal->motor_left_forward(64);
            this->_hal->motor_right_forward(64 - correction);
        }
        else if (correction > 0)
        {
            // We are too far right, so turn left 
            this->_hal->motor_right_forward(64);
            this->_hal->motor_left_forward(64 + correction);
        }
        else
        {
            // No adjustments are required
            this->_hal->motors_forward(64);
        }
    }

    /**
     * Turn the robot left on the spot until the sensors encounter another
     * line
     */
    void LineFollowing::turn_left()
    {
    }

    /**
     * Turn the robot right on the spot until the sensors detect another
     * line
     */
    void LineFollowing::turn_right()
    {
        // 1. Set the right motor to 0
        this->_hal->motor_right_forward(0);
        
        // 2. Set the left motor to full speed
        this->_hal->motor_left_forward(127);
        
        // 3. Poll the inner right sensor for losing the line
        bool lost = false;
        while(!lost) {
            const LineSensors lf = this->_hal->line_following_sensors();
            if(lf.line_right == NO_LINE) {
                lost = true;
            }
        }
        
        // 4. Poll the inner right sensor for regaining the line
        while(lost) {
            const LineSensors lf = this->_hal->line_following_sensors();
            if(lf.line_right == LINE) {
                lost = false;
            }
        }

        // 5. Turn completed, return control to normal line following
    }

    /**
     * Spin the robot 180 degrees, using the line behind to detect when
     * the correct amount of rotation has been reached
     */
    void LineFollowing::turn_around()
    {
    }
}

