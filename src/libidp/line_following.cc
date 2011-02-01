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
    LineFollowing::LineFollowing(const HardwareAbstractionLayer* _hal)
    {
        std::cout << "[LineFollowing] Initialising a Line Follower" << std::endl;
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
            // Junction
            _hal->motors_stop();
        }
        else if (sensors.line_left == LINE && sensors.line_right == NO_LINE)
        {
            // Need to turn left
            _error--;
        }
        else if (sensors.line_left == NO_LINE && sensors.line_right == LINE)
            // Need to turn right
            _error++;
        }
        else if (sensors.line_left == NO_LINE && sensors.line_right == NO_LINE)
        {
            // On the line, continue
        }

        // Call correct_steering to carry out any required adjustments
        correct_steering(_error);
    }

    /**
     * Correct the steering of the robot
     * \param _error A signed integer where negative is too far left, and
     * positive is too far right
     */
    void LineFollowing::correct_steering(int _error)
    {
        std::cout << "[LineFollowing] Correcting steering with error " << _error
            << std::endl;
        if (_error < 0)
        {
            // We are too far right, so turn left
            _hal->motors_turn_left(ki * _error);
        }
        else if (_error > 0)
        {
            // We are too far left, so turn right
            _hal->motors_turn_right(ki * _error);
        }
        else
        {
            // No adjustments are required
            _hal->motors_forward(64);
        }
    }
}

