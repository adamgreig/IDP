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
     * Maximum differential correction value before it gets capped
     */
    const short unsigned int MAX_CORRECTION = 127;

    /**
     * Baseline integral gain for full speed operation
     */
    const double BASELINE_INTEGRAL_GAIN = 5.0;

    /**
     * Baseline LOST timeout for full speed straight line navigation
     */
    const short unsigned int BASELINE_STRAIGHT_TIMEOUT = 50;

    /**
     * Baseline LOST timeout for full speed turning actions
     */
    const short unsigned int BASELINE_TURN_TIMEOUT = 200;

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
            LineFollowingStatus turn_left(
                    unsigned short int skip_lines = 0);
            LineFollowingStatus turn_right(
                    unsigned short int skip_lines = 0);
            LineFollowingStatus turn_around_cw(
                    unsigned short int skip_lines = 0);
            LineFollowingStatus turn_around_ccw(
                    unsigned short int skip_lines = 0);
            LineFollowingStatus turn_around_delivery();
            LineFollowingStatus junction_status(void);
            void set_speed(unsigned short int speed);

        private:
            void correct_steering(void);
            void set_motors_turning(LineFollowingTurnDirection dir);
            LineFollowingStatus turn(LineFollowingTurnDirection dir,
                    unsigned short int skip_lines);
            LineFollowingLineStatus line_status(
                LineFollowingTurnDirection dir);

            HardwareAbstractionLayer* _hal; 
            unsigned short int _left_error;
            unsigned short int _right_error;
            unsigned short int _speed;
            bool _lost_turning_line;
            unsigned short int _lost_time;
            unsigned short int _lines_seen;
            double _integral_gain;
            unsigned int _lost_timeout;
            unsigned int _turning_timeout;
    };
}

#endif /* LIBIDP_LINE_FOLLOWING_H */

