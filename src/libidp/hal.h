// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// hal.h
// Hardware Abstraction Layer class definition
//
// Hardware Abstraction Layer - provide a hardware agnostic interface to
// the required hardware functionality

#pragma once
#ifndef LIBIDP_HAL_H
#define LIBIDP_HAL_H

#include <robot_link.h>

namespace IDP {

    static const int MOTOR_MAX_SPEED = 127;
    static const int MOTOR_RAMP_TIME = 16;
    static const bool LINE = true;
    static const bool NO_LINE = false;
    
    struct LineSensors
    {
        bool outer_left;
        bool line_left;
        bool line_right;
        bool outer_right;
    };

    class HardwareAbstractionLayer
    {
        public:
            HardwareAbstractionLayer(const int robot);
            void motors_forward(const unsigned short int speed) const;
            void motors_backward(const unsigned short int speed) const;
            void motor_left_go(const unsigned short int speed) const;
            void motor_right_go(const unsigned short int speed) const;
            void motors_turn_left(const unsigned short int speed) const;
            void motors_turn_right(const unsigned short int speed) const;
            void motors_stop() const;
            void clear_status_register() const;
            const LineSensors line_following_sensors() const;
        private:
            bool check_max_speed(const unsigned short int speed) const;
            robot_link* rlink;
    };
}

#endif /* LIBIDP_HAL_H */

