// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.cc
// Line Following class implementation

#include "line_following.h"

#include <iostream>

#include <robot_instr.h>

#include <hal.h>

namespace IDP {
    /**
     * Construct the Line Follower
     */
    LineFollowing::LineFollowing()
    {
        // Accumulator
        int error;

        if (LineSensors.line_left == LINE && LineSensors.line_right == LINE)
        {
            // Junction
        }
        else if (LineSensors.line_left == LINE && LineSensors.line_right == NO_LINE)
        {
            // Need to turn left
            e--;
        }
        else if (LineSensors.line_left == NO_LINE && LineSensors.line_right == LINE)
            // Need to turn right
            e++;
        }
        else if (LineSensors.line_left == NO_LINE && LineSensors.line_right == NO_LINE)
        {
            // On the line, continue
        }

        // Call correct_steering to carry out any required adjustments
        correct_steering(error);
        
    }

    /**
     * Correct the steering of the robot
     * \param error A signed integer where negative is too far left, and
     * positive is too far right
     */
    void LineFollowing::correct_steering(int error)
    {
        if (error < 0)
        {
            // We are too far right, so turn left
            motors_turn_left(ki * error);
        }
        else if (error > 0)
        {
            // We are too far left, so turn right
            motors_turn_right(ki * error);
        }
        else
        {
            // No adjustments are required
            motors_forward(64);
        }
    }
}

