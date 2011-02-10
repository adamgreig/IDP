// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// line_following.h
// Line Following class definition
//
// Line Following - follow a white line and adjust course as required

#pragma once
#ifndef LIBIDP_LINE_FOLLOWING_H
#define LIBIDP_LINE_FOLLOWING_H

namespace IDP {

    class HardwareAbstractionLayer;

    /**
     * Constant for integral control in line following.
     */
    const double INTEGRAL_GAIN = 4.0;

    /**
     * Maximum differential correction value before it gets capped
     */
    const short unsigned int MAX_CORRECTION = 127;

    /**
     * The number of loop iterations before we count as lost when driving
     * straight
     */
    const unsigned int LOST_TIMEOUT = 50;

    /**
     * The number of loop iterations before we count as lost when turning
     * around
     */
    const unsigned int LOST_TURNING_TIMEOUT = 400;

    /**
     * How much an outer sensor seeing the edge of a line should add
     * to the appropriate error
     */
    const unsigned int EDGE_ERROR = 2;

    /**
     * Line following return status codes.
     *
     * ACTION_IN_PROGRESS indicates that the requested action is still
     * underway.
     *
     * ACTION_COMPLETED indicates that the requested action has finished.
     *
     * LEFT_TURN_FOUND, RIGHT_TURN_FOUND and BOTH_TURNS_FOUND indicate that
     * possible turns have been found in the path.
     *
     * LOST indicates that no line could be seen on any sensors and that this
     * is unexpected.
     */
    enum LineFollowingStatus {
        ACTION_IN_PROGRESS,
        ACTION_COMPLETED,
        LEFT_TURN_FOUND,
        RIGHT_TURN_FOUND,
        BOTH_TURNS_FOUND,
        LOST,
        NO_TURNS_FOUND
    };

    /**
     * Possible turn directions
     */
    enum LineFollowingTurnDirection {
        TURN_LEFT,
        TURN_RIGHT,
        TURN_AROUND_CW,
        TURN_AROUND_CCW,
        MAX_TURN_DIRECTION
    };

    /**
     * Possible line statuses, used internally.
     */
    enum LineFollowingLineStatus {
        ON_LINE,
        LOST_LINE,
        OTHER,
        MAX_LINE_STATUS
    };

    /**
     * String representations of LineFollowingStatus
     */
    static const char* const LineFollowingStatusStrings[] = {
        "ACTION_IN_PROGRESS",
        "ACTION_COMPLETED",
        "LEFT_TURN_FOUND",
        "RIGHT_TURN_FOUND",
        "BOTH_TURNS_FOUND",
        "LOST",
        "NO_TURNS_FOUND"
    };

    /**
     * String representations of LineFollowingTurnDirection
     */
    static const char* const LineFollowingTurnDirectionStrings[] = {
        "TURN_LEFT",
        "TURN_RIGHT",
        "TURN_AROUND_CW",
        "TURN_AROUND_CCW",
        "MAX_TURN_DIRECTION"
    };

    /**
     * String representation of LineFollowingLineStatus
     */
    static const char* const LineFollowingLineStatusStrings[] = {
        "ON_LINE",
        "LOST_LINE",
        "OTHER",
        "MAX_LINE_STATUS"
    };


    /**
     * Cap a line following correction value to MAX_CORRECTION
     * \param correction The existing correction value
     * \returns The capped correction value
     */
    unsigned short int cap_correction(const unsigned short int correction);

    /**
     * Maintain the robot position correctly with respect to the white
     * line markers, during driving and manouvering
     */
    class LineFollowing 
    {
        public:
            LineFollowing(HardwareAbstractionLayer* hal);
            LineFollowingStatus follow_line(void);
            LineFollowingStatus turn_left(void);
            LineFollowingStatus turn_right(void);
            LineFollowingStatus turn_around_cw(void);
            LineFollowingStatus turn_around_ccw(void);
            LineFollowingStatus junction_status(void);
            void set_speed(unsigned short int speed);

        private:
            void correct_steering(void);
            void set_motors_turning(LineFollowingTurnDirection dir);
            LineFollowingStatus turn(LineFollowingTurnDirection dir);
            LineFollowingLineStatus line_status(
                LineFollowingTurnDirection dir);

            HardwareAbstractionLayer* _hal; 
            unsigned short int _left_error;
            unsigned short int _right_error;
            unsigned short int _speed;
            bool _lost_turning_line;
            unsigned short int _lost_time;
    };
}

#endif /* LIBIDP_LINE_FOLLOWING_H */

