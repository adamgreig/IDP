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

    const int MOTOR_MAX_SPEED = 127;
    const int MOTOR_RAMP_TIME = 16;
    const bool LINE = true;
    const bool NO_LINE = false;
    
    /**
     * Contains the LINE or NO_LINE status of each of the four 
     * IR sensors used for line following
     */
    struct LineSensors
    {
        bool outer_left;
        bool line_left;
        bool line_right;
        bool outer_right;
    };

    /**
     * Provide a hardware agnostic interface to the required hardware
     * functionality
     */
    class HardwareAbstractionLayer
    {
        public:
            HardwareAbstractionLayer(const int robot);
            void motors_forward(const unsigned short int speed) const;
            void motors_backward(const unsigned short int speed) const;
            void motor_left_forward(const unsigned short int speed) const;
            void motor_right_forward(const unsigned short int speed) const;
            void motor_left_backward(const unsigned short int speed) const;
            void motor_right_backward(const unsigned short int speed) const;
            void motors_turn_left(const unsigned short int speed) const;
            void motors_turn_right(const unsigned short int speed) const;
            void motors_stop() const;
            char status_register() const;
            void clear_status_register() const;
            const LineSensors line_following_sensors() const;
            bool reset_switch() const;
            bool grabber_switch() const;
            unsigned short int colour_ldr() const;
            unsigned short int bad_bobbin_ldr() const;
            void indication_LEDs(const bool led_0, const bool led_1,
                const bool led_2) const;
            void colour_leds(const bool red, const bool green) const;
            void bad_bobbin_led(const bool status) const;
            void grabber_jaw(const bool status) const;
            void grabber_lift(const bool status) const;
        private:
            bool check_max_speed(const unsigned short int speed) const;
            robot_link* rlink;
    };
}

#endif /* LIBIDP_HAL_H */

