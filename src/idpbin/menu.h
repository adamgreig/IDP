// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// menu.h
// Menu class definition
//
// Menu - a menu system to select from a variety of robot tasks

#pragma once
#ifndef IDPBIN_MENU_H
#define IDPBIN_MENU_H

namespace IDP {

    /**
     * The list of all possible menu choices.
     */
    enum MenuChoice {
        MENU_QUIT, MENU_BACK, MENU_RUN_MAIN_TASK, MENU_RUN_ALL_SELF_TESTS,
        MENU_LINE_FOLLOWING_TEST, MENU_DRIVE_FORWARD, MENU_DRIVE_BACKWARD,
        MENU_TURN_LEFT, MENU_TURN_RIGHT, MENU_STEER_LEFT, MENU_STEER_RIGHT,
        MENU_STOP, MENU_ANALYSE, MENU_CLAMP_TESTS, MENU_LINE_SENSORS,
        MENU_SWITCHES, MENU_LDRS, MENU_NAVIGATION_TEST, MENU_INDICATOR_LEDS,
        MENU_ACTUATORS, MENU_COLOUR_SENSOR_LEDS, MENU_BAD_BOBBIN_LED,
        MENU_NAVIGATE_TO_BOBBIN, MENU_NAVIGATE_TO_BOX, MENU_BOBBIN_PRESENT,
        MENU_NAVIGATE_TO_DELIVERY, MENU_DELIVERY, MENU_BOX_PRESENT,
        MAX_MENU_CHOICE
    };

    /**
     * A menu organisation class to allow easy implementation and later
     * additions to a multi-level menu system.
     *
     * get_choice will eventually return a MenuChoice but does so by calling
     * appropriate sublevel functions which in turn each pass back up a
     * MenuChoice, allowing sublevels to be implemented somewhat independently
     * of the higher level menus.
     */
    class Menu
    {
        public:
            static MenuChoice get_choice();
        private:
            static MenuChoice self_tests();
            static MenuChoice motor_control();
            static MenuChoice navigation();
            static MenuChoice clamp_control();
            static MenuChoice sensor_tests();
            static MenuChoice output_tests();
            static MenuChoice task_tests();
    };
}

#endif /* IDPBIN_MENU_H */
