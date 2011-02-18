// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.cc
// Line Following class implementation

#include "hal.h"
#include "line_following.h"

// Debug functionality
#define MODULE_NAME "LineFollowing"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   false
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {

    /**
     * Construct the Line Follower
     */
    LineFollowing::LineFollowing(HardwareAbstractionLayer* hal)
        : _hal(hal), _left_error(0), _right_error(0), _speed(0),
        _lost_turning_line(false), _lost_time(0), _lines_seen(0),
        _integral_gain(5.0), _lost_timeout(50), _turning_timeout(400)
    {
        INFO("Initialising a Line Follower");
        TRACE("LineFollowing(" << hal << ")");
    }

    /**
     * Read line sensors and correct motor movement to keep us going straight.
     *
     * \returns A LineFollowingStatus to indicate that either we are going
     * fine, we are lost, or one or more possible turns were found.
     */
    LineFollowingStatus LineFollowing::follow_line() {
        TRACE("follow_line()");

        // Quick speed sanity check to prevent otherwise very frustrating
        // mistakes
        if(this->_speed == 0) {
            ERROR("follow_line() called but speed is 0!");
        }

        // We're not turning, so reset the state in case it's incorrect
        this->_lost_turning_line = false;
        this->_lines_seen = 0;

        // Read the state of the IR sensors from hal
        const LineSensors s = _hal->line_following_sensors();

        // Take various appropriate action depending on sensor state.
        // A long if statement but at least it's not very deeply nested.
        if( s.line_left == LINE && s.line_right == LINE &&
            s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We are okay! Keep driving forwards
            DEBUG("Driving forwards");
            this->_left_error = this->_right_error = this->_lost_time  = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == NO_LINE && s.line_right == LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Have veered a little left, compensate
            DEBUG("Compensating for slight left drift");
            this->_left_error++;
            this->_right_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Have veered a little right, compensate
            DEBUG("Compensating for slight right drift");
            this->_right_error++;
            this->_lost_time = 0;
            this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == LINE && s.outer_right == NO_LINE)
        {
            // We've veered a lot right, compensate
            DEBUG("Compensating for large right drift");
            this->_right_error += EDGE_ERROR;
            this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == LINE)
        {
            // We've veered a lot left, compensate
            DEBUG("Compensating for large left drift");
            this->_left_error += EDGE_ERROR;
            this->_right_error = this->_lost_time = 0;
            this->correct_steering();
            return ACTION_IN_PROGRESS;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == LINE && s.outer_right == NO_LINE)
        {
            // Found a left turn
            INFO("Left turn found");
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return LEFT_TURN_FOUND;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == NO_LINE && s.outer_right == LINE)
        {
            // Found a right turn
            INFO("Right turn found");
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return RIGHT_TURN_FOUND;
        } else if(  s.line_left == LINE && s.line_right == LINE &&
                    s.outer_left == LINE && s.outer_right == LINE)
        {
            // Found both turns
            INFO("Both turns found");
            this->_right_error = this->_left_error = this->_lost_time = 0;
            this->correct_steering();
            return BOTH_TURNS_FOUND;
        } else if(  s.line_left == NO_LINE && s.line_right == NO_LINE &&
                    s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // We can't see any lines. If it has been a long time since
            // we were on a line, return LOST, otherwise correct towards
            // the last known direction.
            DEBUG("No line visible");
            this->_lost_time++;
            if(this->_lost_time > this->_lost_timeout) {
                // Prevent lost_time from overflowing
                INFO("Haven't seen a line for a while, LOST");
                this->_lost_time--;
                this->correct_steering();
                return LOST;
            } else {
                DEBUG("Trying to find the line");
                if(this->_left_error)
                    this->_left_error++;
                else if(this->_right_error)
                    this->_right_error++;
                this->correct_steering();
                return ACTION_IN_PROGRESS;
            }
        } else {
            // Something odd happened. Keep driving as we were.
            DEBUG("In an unhandled state, continuing steering as before");
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
        TRACE("correct_steering()");

        // Calculate available headroom for applying a forward
        // correction
        unsigned short int headroom = MOTOR_MAX_SPEED - this->_speed;

        // Switch based on error direction
        if(_left_error) {
            DEBUG("Correcting a left error");

            // Calculate the required differential correction
            unsigned short int correction = static_cast<unsigned short int>(
                static_cast<double>(_left_error) * this->_integral_gain);
            
            DEBUG("Pre-cap correction: " << correction);

            // Cap correction
            correction = cap_correction(correction);

            DEBUG("Post-cap correction: " << correction);

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
            DEBUG("Correcting a right error");

            // Calculate the required differential correction
            unsigned short int correction = static_cast<unsigned short int>(
                static_cast<double>(_right_error) * this->_integral_gain);
            
            DEBUG("Pre-cap correction: " << correction);

            // Cap correction
            correction = cap_correction(correction);

            DEBUG("Post-cap correction: " << correction);

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
            DEBUG("No correction required");
            this->_hal->motors_forward(this->_speed);
        }
    }

    /**
     * Turn the robot left until the sensors encounter another line
     * \param skip_lines How many lines we should detect and skip over
     * \returns A LineFollowingStatus code
     */
    LineFollowingStatus LineFollowing::turn_left(
            unsigned short int skip_lines)
    {
        TRACE("turn_left(" << skip_lines << ")");
        return this->turn(TURN_LEFT, skip_lines);
    }

    /**
     * Turn the robot right until the sensors detect another line
     * \param skip_lines How many lines we should detect and skip over
     * \returns A LineFollowingStatus code
     */
    LineFollowingStatus LineFollowing::turn_right(
            unsigned short int skip_lines)
    {
        TRACE("turn_right(" << skip_lines << ")");
        return this->turn(TURN_RIGHT, skip_lines);
    }

    /**
     * Turn the robot around clockwise until the sensors detect another line
     * \param skip_lines How many lines we should detect and skip over
     * \returns A LineFollowingStatus code
     */
    LineFollowingStatus LineFollowing::turn_around_cw(
            unsigned short int skip_lines)
    {
        TRACE("turn_around_cw(" << skip_lines << ")");
        return this->turn(TURN_AROUND_CW, skip_lines);
    }

    /**
     * Turn the robot around counterclockwise until the sensors detect another
     * line
     * \param skip_lines How many lines we should detect and skip over
     * \returns A LineFollowingStatus code
     */
    LineFollowingStatus LineFollowing::turn_around_ccw(
            unsigned short int skip_lines)
    {
        TRACE("turn_around_ccw(" << skip_lines << ")");
        return this->turn(TURN_AROUND_CCW, skip_lines);
    }

    /**
     * Turn around, but stop as soon as we lose the line
     */
    LineFollowingStatus LineFollowing::turn_around_delivery()
    {
        TRACE("turn_around_delivery()");
        LineFollowingStatus status = this->turn(TURN_AROUND_CW, 0);
        if(this->_lost_turning_line) {
            this->_lost_turning_line = false;
            this->_lost_time = 0;
            return ACTION_COMPLETED;
        } else {
            return status;
        }
    }

    /**
     * Return whether we can see a junction or not, without
     * changing motor settings.
     * \returns A LineFollowingStatus indicating junctions or
     * NO_TURNS_FOUND if no junctions found.
     */
    LineFollowingStatus LineFollowing::junction_status()
    {
        TRACE("junction_status()");

        // Read the state of the IR sensors from hal
        const LineSensors s = _hal->line_following_sensors();

        // If the inner sensors do not detect a line, it implies we
        // are not really at a junction anyway, but maybe have drifted
        // far enough to a side that we think the main line is a junction.
        // Therefore, return early in this case.
        //
        // Otherwise, we check the outer sensors for junctions.
        if(s.line_left == NO_LINE || s.line_right == NO_LINE) {
            DEBUG("Not looking at edge sensors as inner sensors are NO_LINE");
            return NO_TURNS_FOUND;
        } else if(s.outer_left == LINE && s.outer_right == LINE) {
            INFO("Found both turns");
            return BOTH_TURNS_FOUND;
        } else if(s.outer_left == LINE) {
            INFO("Found left turn");
            return LEFT_TURN_FOUND;
        }  else if(s.outer_right == LINE) {
            INFO("Found right turn");
            return RIGHT_TURN_FOUND;
        } else {
            DEBUG("No turns found");
            return NO_TURNS_FOUND;
        }
    }

    /**
     * Set the speed that motors will be driven at
     * \param speed How fast to drive the motors, 0 to MOTOR_MAX_SPEED.
     */
    void LineFollowing::set_speed(unsigned short int speed)
    {
        TRACE("set_speed(" << speed << ")");
        DEBUG("set_speed called with speed=" << speed);

        if(speed < MOTOR_MAX_SPEED) {
            INFO("Setting motor speed to " << speed);
            this->_speed = speed;
        } else {
            INFO("Setting motor speed to MOTOR_MAX_SPEED (" <<
                MOTOR_MAX_SPEED << ")");
            this->_speed = MOTOR_MAX_SPEED;
        }

        // Update the gain of LineFollowing's integral controller
        // to compensate for the new speed
        unsigned short int diff = MOTOR_MAX_SPEED - this->_speed;
        double new_gain = 5.0 - (static_cast<double>(diff) / 36.0);
        DEBUG("Setting new gain to " << new_gain);
        this->_integral_gain = new_gain;

        // Update the LOST_TIMEOUT loop iterations to account for
        // the change in robot speed
        unsigned int new_timeout = BASELINE_STRAIGHT_TIMEOUT + (diff/5);
        DEBUG("Setting new LOST timeout to " << new_timeout);
        this->_lost_timeout = new_timeout;

        // Now update the LOST_TIMEOUT for turning actions
        new_timeout = BASELINE_TURN_TIMEOUT + (diff/5);
        DEBUG("Setting new LOST TURNING timeout to " << new_timeout);
        this->_turning_timeout = new_timeout;
    }

    /**
     * Return the current line status, depending on turning direction.
     * \param dir The turning direction
     * \returns a LineFollowingLineStatus
     */
    LineFollowingLineStatus LineFollowing::line_status(
        LineFollowingTurnDirection dir)
    {
        TRACE("line_status(" << LineFollowingTurnDirectionStrings[dir] << ")");

        // Read the IR sensors from HAL
        const LineSensors s = _hal->line_following_sensors();
        
        if(s.line_left == LINE && s.line_right == LINE &&
            s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Dead on the line
            DEBUG("Line status: We are perfectly on the line");
            return ON_LINE;
        } else if(s.line_left == NO_LINE && s.line_right == NO_LINE &&
                  s.outer_left == NO_LINE && s.outer_right == NO_LINE)
        {
            // Totally off the line
            DEBUG("Line status: We cannot see the line at all");
            return LOST_LINE;
        } else {
            // Different cases for different directions
            if(
                (dir == TURN_LEFT &&
                    (s.line_left == LINE && s.line_right == NO_LINE)) ||
                (dir == TURN_RIGHT &&
                    (s.line_left == NO_LINE && s.line_right == LINE)) ||
                (dir == TURN_AROUND_CW &&
                    (s.outer_right == LINE)) ||
                (dir == TURN_AROUND_CCW &&
                    (s.line_left == LINE && s.line_right == NO_LINE))
            ) {
                DEBUG("Line status: on the line for our turning direction");
                return ON_LINE;
            } else {
                DEBUG("Line status: other");
                return OTHER;
            }
        }

        // We shouldn't ever get here but the compiler can get confused.
        return OTHER;
    }

    /**
     * Perform a turn in the given direction.
     *
     * turn_left, turn_right, turn_around_cw and turn_around_ccw are
     * all essentially just wrappers for this functions. They stop it
     * being called with silly arguments, at least.
     *
     * \param dir The direction to turn in
     * \param skip_lines How many lines we should detect and skip over
     */
    LineFollowingStatus LineFollowing::turn(LineFollowingTurnDirection dir,
            unsigned short int skip_lines)
    {
        TRACE("turn(" << LineFollowingTurnDirectionStrings[dir] << ")");
        INFO("Executing a " << LineFollowingTurnDirectionStrings[dir]);

        // Set the motors going
        this->set_motors_turning(dir);

        // Check the current line status for this turn direction
        LineFollowingLineStatus status = this->line_status(dir);

        if(status == ON_LINE) {
            // If we are on the line, we have either just started to turn
            // and not yet moved much, so keep turning, or have re-found
            // the line after turning, so indicate success.
            DEBUG("Turning, currently on the line");
            if(!this->_lost_turning_line) {
                DEBUG("Still starting the turn");
                return ACTION_IN_PROGRESS;
            } else {
                INFO("Found a line again");
                this->_lost_turning_line = false;
                if(this->_lines_seen < skip_lines) {
                    INFO("Skipping this line");
                    this->_lines_seen++;
                    return ACTION_IN_PROGRESS;
                } else {
                    INFO("Found final line, ending turn");
                    this->_lines_seen = 0;
                    return ACTION_COMPLETED;
                }
            }
        } else if(status == LOST_LINE) {
            // If we've lost the line, we are properly into the turn now.
            DEBUG("Turning, cannot see the line");

            if(!this->_lost_turning_line) {
                INFO("Lost the turning line");
                this->_lost_turning_line = true;
            }

            // Increment the lost_time counter so if we don't find the line
            // we can think about recovering rather than going in circles
            // forever.
            this->_lost_time++;
            if(this->_lost_time > this->_turning_timeout) {
                ERROR("Haven't seen the line for ages while turning, LOST");
                this->_lost_time--; //prevent overflow
                return LOST;
            }

            return ACTION_IN_PROGRESS;
        } else {
            // Other states are acceptable and mostly transition states.
            // Keep on turning.
            DEBUG("Turning, in an unhandled state");
            return ACTION_IN_PROGRESS;
        }
    } 

    /**
     * Set the motors to the correct steering speeds.
     * \param dir Which direction to turn in
     *
     * This will set the left and right motors either forwards,
     * backwards or stationary as appropriate to execute a turn.
     */
    void LineFollowing::set_motors_turning(LineFollowingTurnDirection dir)
    {
        TRACE("set_motors_turning(" << LineFollowingTurnDirectionStrings[dir]
            << ")");

        if(dir == TURN_LEFT) {
            DEBUG("Steering left");
            this->_hal->motor_left_forward(0);
            this->_hal->motor_right_forward(this->_speed);
        } else if(dir == TURN_RIGHT) {
            DEBUG("Steering right");
            this->_hal->motor_right_forward(0);
            this->_hal->motor_left_forward(this->_speed);
        } else if(dir == TURN_AROUND_CW) {
            DEBUG("Steering around, clockwise");
            this->_hal->motor_right_backward(this->_speed / 2);
            this->_hal->motor_left_forward(this->_speed / 2);
        } else if(dir == TURN_AROUND_CCW) {
            DEBUG("Steering around, anticlockwise");
            this->_hal->motor_right_forward(this->_speed / 2);
            this->_hal->motor_left_backward(this->_speed / 2);
        }
    }

    // Documented in line_following.h
    // Intentionally not part of the class.
    unsigned short int cap_correction(const unsigned short int correction) {
        if(correction > MAX_CORRECTION)
            return MAX_CORRECTION;
        else
            return correction;
    }
}

