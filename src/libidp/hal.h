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

    /**
     * Highest allowable motor speed in either direction
     */
    const int MOTOR_MAX_SPEED = 127;

    /**
     * How fast to ramp the motors towards the desired speed.
     * Lower is faster.
     */
    const int MOTOR_RAMP_TIME = 16;

    /**
     * Line sensor status, LINE or NO_LINE.
     */
    enum LineSensorStatus {
        LINE,
        NO_LINE
    };
    
    /**
     * Contains the LINE or NO_LINE status of each of the four 
     * IR sensors used for line following
     */
    struct LineSensors
    {
        LineSensorStatus outer_left;
        LineSensorStatus line_left;
        LineSensorStatus line_right;
        LineSensorStatus outer_right;
    };

    /**
     * Provide a hardware agnostic interface to the required hardware
     * functionality
     */
    class HardwareAbstractionLayer
    {
        public:
            HardwareAbstractionLayer(const int robot);
            void motors_forward(const unsigned short int speed);
            void motors_backward(const unsigned short int speed);
            void motor_left_forward(const unsigned short int speed);
            void motor_right_forward(const unsigned short int speed);
            void motor_left_backward(const unsigned short int speed);
            void motor_right_backward(const unsigned short int speed);
            void motors_turn_left(const unsigned short int speed);
            void motors_turn_right(const unsigned short int speed);
            void motors_stop();
            char status_register() const;
            void clear_status_register() const;
            const LineSensors line_following_sensors() const;
            bool reset_switch() const;
            bool grabber_switch() const;
            unsigned short int colour_ldr() const;
            unsigned short int bad_bobbin_ldr() const;
            void indication_LEDs(const bool led_0, const bool led_1,
                const bool led_2);
            void colour_LEDs(const bool red, const bool green);
            void bad_bobbin_LED(const bool status);
            void grabber_jaw(const bool status);
            void grabber_lift(const bool status);
        private:
            bool check_max_speed(const unsigned short int speed) const;
            robot_link* rlink;
            unsigned short int _port7;
    };
}

#endif /* LIBIDP_HAL_H */

