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

#include <robot_link.h>

namespace IDP {

    class HardwareAbstractionLayer;

    // Constant for integral control in line following
    const double ki = 4.0;

    /**
     * Maintain the robot position correctly with respect to the white
     * line markers, during driving and manouvering
     */
    class LineFollowing 
    {
        public:
            LineFollowing(const HardwareAbstractionLayer* hal);
            void correct_steering(int error);
            void follow_line();

        private:
            int _error;
            const HardwareAbstractionLayer* _hal; 
    };
}

#endif /* LIBIDP_LINE_FOLLOWING_H */

