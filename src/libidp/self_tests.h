// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// self_tests.h
// Self Tests class definition
//
// Self Tests - tests for all attached hardware to ensure
// they are behaving as expected

#pragma once
#ifndef LIBIDP_SELF_TESTS_H
#define LIBIDP_SELF_TESTS_H

#include <robot_link.h>

namespace IDP {

    class HardwareAbstractionLayer;

    /**
     * Execute a variety of functionality self tests
     */
    class SelfTests
    {
        public:
            SelfTests(int robot);
            ~SelfTests();
            void drive_forward(void);
            void drive_backward(void);
            void stop(void);
            void turn_left(void);
            void turn_right(void);
            void steer_left(void);
            void steer_right(void);
            void line_sensors(void);
            void microswitches(void);
            void LDRs(void);
            void actuators(void);
            void line_following(void);
            void clamp_control(void);
            void bobbin_analyse(void);
            void navigate(void);
            void position(void);
            void status_LEDs(void);
            void colour_sensor_LEDs(void);
            void badness_LED(void);
        private:
            int _robot;
            HardwareAbstractionLayer* _hal;
    };
}

#endif /* LIBIDP_SELF_TESTS_H */

