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
     * The number of loop iterations before we count as lost
     */
    const unsigned int LOST_TIMEOUT = 50;

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
        LOST
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
            LineFollowing(const HardwareAbstractionLayer* hal);
            LineFollowingStatus follow_line(void);
            LineFollowingStatus turn_left(void);
            LineFollowingStatus turn_right(void);
            void set_speed(unsigned short int speed);

        private:
            void correct_steering(void);
            unsigned short int _left_error;
            unsigned short int _right_error;
            bool _lost_turning_line;
            unsigned short int _speed;
            unsigned short int _lost_time;
            const HardwareAbstractionLayer* _hal; 
    };
}

#endif /* LIBIDP_LINE_FOLLOWING_H */

