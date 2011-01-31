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

    // Constant for integral control in line following
    const int ki = 10;

    class LineFollowing 
    {
        public:
            LineFollowing();
            void correct_steering(int error);

        private:
            int error;
    };
}

#endif /* LIBIDP_LINE_FOLLOWING_H */

