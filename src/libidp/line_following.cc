// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.cc
// Line Following class implementation

#include <iostream>

#include "hal.h"
#include "line_following.h"

namespace IDP {
    /**
     * Construct the Line Follower
     */
    LineFollowing::LineFollowing(const HardwareAbstractionLayer* hal)
        : _hal(hal), _left_error(0), _right_error(0), _speed(0),
        _lost_turning_line(false), _lost_time(0)
    {
        std::cout << "[LineFollowing] Initialising a Line Follower";
        std::cout << std::endl;
    }

    /**
     * Read line sensors and correct motor movement to keep us going straight.
     *
     * \returns A LineFollowingStatus to indicate that either we are going
     * fine, we are lost, or one or more possible turns were found.
     */
    LineFollowingStatus LineFollowing::follow_line() {
        std::cout << "[LineFollowing] Following a line straight forwards" << std::endl;

        // We're not turning
        this->_lost_turning_line = false;

        // Read the state of the IR sensors from hal
        const LineSensors s = _hal->line_following_sensors();

        // Take various appropriate action depending on sensor state
        if( s.line_left == LINE && s.line_right == LINE &&
            s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We are okay! Keep driving forwards
            this->_left_error = this->_right_error = this->_lost_time  = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == NO_LINE && s.line_right == LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Have veered a little left, compensate
            this->_left_error++;
            this->_right_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Have veered a little right, compensate
            this->_right_error++;
            this->_lost_time = 0;
            this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == LINE && s.outer_right == NO_LINE)
        {
            // Found a left turn
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return LEFT_TURN_FOUND;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == NO_LINE && s.outer_right == LINE)
        {
            // Found a right turn
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return RIGHT_TURN_FOUND;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == LINE && s.outer_right == LINE)
        {
            // Found both turns
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return BOTH_TURNS_FOUND;
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We can't see any lines. If it has been a long time since
            // we were on a line, return LOST, otherwise correct towards
            // the last known direction.
            this->_lost_time++;
            if(this->_lost_time > LOST_TIMEOUT) {
                // Prevent lost_time from overflowing
                this->_lost_time--;
                this->correct_steering();
                return LOST;
            } else {
                if(this->_left_error)
                    this->_left_error++;
                else if(this->_right_error)
                    this->_right_error++;
                this->correct_steering();
                return ACTION_IN_PROGRESS;
            }
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == LINE && s.outer_right == NO_LINE)
        {
            // We've veered a lot right, compensate
            this->_right_error += EDGE_ERROR;
            this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == LINE)
        {
            // We've veered a lot left, compensate
            this->_left_error += EDGE_ERROR;
            this->_right_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else {
            // Something odd happened. Keep driving as we were.
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        }
    }

    /**
     * Correct the steering of the robot after the error has been
     * calculated in follow_line.
     */
    void LineFollowing::correct_steering()
    {
        // Calculate available headroom for applying a forward
        // correction
        unsigned short int headroom = MOTOR_MAX_SPEED - this->_speed;

        // Switch based on error direction
        if(_left_error) {
            // Calculate the required differential correction
            unsigned short int correction = static_cast<unsigned short int>(
                static_cast<double>(_left_error) * INTEGRAL_GAIN);

            // Cap correction
            correction = cap_correction(correction);

            // Apply as much correction as possible to the left
            // wheel forwards and whatever is left to right backwards
            if(headroom >= correction) {
                this->_hal->motor_left_forward(this->_speed + correction);
                this->_hal->motor_right_forward(this->_speed);
            } else {
                this->_hal->motor_left_forward(this->_speed + headroom);
                this->_hal->motor_right_forward(
                    this->_speed - (correction - headroom));
            }

        } else if(_right_error) {
            // Calculate the required differential correction
            unsigned short int correction = static_cast<unsigned short int>(
                static_cast<double>(_right_error) * INTEGRAL_GAIN);

            // Cap correction
            correction = cap_correction(correction);

            // Apply as much correction as possible to the right
            // wheel forwards and whatever is right to left backwards
            if(headroom >= correction) {
                this->_hal->motor_right_forward(this->_speed + correction);
                this->_hal->motor_left_forward(this->_speed);
            } else {
                this->_hal->motor_right_forward(this->_speed + headroom);
                this->_hal->motor_left_forward(
                    this->_speed - (correction - headroom));
            }
        } else {
            // No correction required so drive forwards
            this->_hal->motors_forward(this->_speed);
        }
    }

    /**
     * Turn the robot left until the sensors encounter another line
     */
    LineFollowingStatus LineFollowing::turn_left()
    {
        std::cout << "[LineFollowing] Executing a left turn" << std::endl;

        // Read the state of the IR sensors from hal
        const LineSensors s = _hal->line_following_sensors();

        this->_hal->motor_left_forward(0);
        this->_hal->motor_right_forward(this->_speed);

        if((s.line_left == LINE && s.line_right == LINE &&
            s.outer_left == NO_LINE && s.outer_right == NO_LINE) ||
           (s.line_left == NO_LINE && s.line_right == LINE))
        {
            // If we've not lost the line, continue starting the turn
            // Otherwise we have now finished
            if(!this->_lost_turning_line) {
                return ACTION_IN_PROGRESS;
            } else {
                return ACTION_COMPLETED;
            }
        } else if(s.line_left == NO_LINE && s.line_right == NO_LINE &&
                  s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We've now lost the line
            this->_lost_turning_line = true;
        } else {
            // Something is wrong. Hope it gets better.
            return ACTION_IN_PROGRESS;
        }
    }

    /**
     * Turn the robot right until the sensors detect another line
     */
    LineFollowingStatus LineFollowing::turn_right()
    {
        std::cout << "[LineFollowing] Executing a right turn" << std::endl;

        // Read the state of the IR sensors from hal
        const LineSensors s = _hal->line_following_sensors();

        this->_hal->motor_right_forward(0);
        this->_hal->motor_left_forward(this->_speed);

        if((s.line_left == LINE && s.line_right == LINE &&
            s.outer_left == NO_LINE && s.outer_right == NO_LINE) ||
           (s.line_left == LINE && s.line_right == NO_LINE))
        {
            // If we've not lost the line, continue starting the turn
            // Otherwise we have now finished
            if(!this->_lost_turning_line) {
                std::cout << "[LineFollowing] At start of turn" << std::endl;
                return ACTION_IN_PROGRESS;
            } else {
                std::cout << "[LineFollowing] Found line again" << std::endl;
                this->_lost_turning_line = false;
                return ACTION_COMPLETED;
            }
        } else if(s.line_left == NO_LINE && s.line_right == NO_LINE &&
                  s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We've now lost the line
            std::cout << "[LineFollowing] Lost turning line" << std::endl;
            this->_lost_turning_line = true;
            return ACTION_IN_PROGRESS;
        } else {
            // Something is wrong. Hope it gets better.
            return ACTION_IN_PROGRESS;
        }
    }

    /**
     * Set the speed that motors will be driven at
     * \param speed How fast to drive the motors, 0 to MOTOR_MAX_SPEED.
     */
    void LineFollowing::set_speed(unsigned short int speed)
    {
        std::cout << "[LineFollowing] Setting LineFollowing speed to ";
        std::cout << speed << std::endl;

        if(speed < MOTOR_MAX_SPEED)
            this->_speed = speed;
        else
            this->_speed = MOTOR_MAX_SPEED;
    }

    unsigned short int cap_correction(const unsigned short int correction) {
        if(correction > MAX_CORRECTION)
            return MAX_CORRECTION;
        else
            return correction;
    }
}

