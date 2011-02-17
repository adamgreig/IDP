// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// menu.cc
// Menu class implementation

#include "menu.h"
#include <iostream>

namespace IDP {
    
    /**
     * Top level menu entrance. Will eventually return a selection.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::get_choice()
    {
        // Loop until broken by a return statement
        while(true) {
            std::cout << std::endl;
            std::cout << "Welcome to Fluffy Mission Control, brought to you";
            std::cout << std::endl;
            std::cout << "by The El Thetas. What would you like to do?";
            std::cout << std::endl << std::endl;
            std::cout << "1) Run Main Task" << std::endl;
            std::cout << "2) Run Self Tests" << std::endl;
            std::cout << "q) Quit" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);

            if(std::cin.eof()) {
                // Deal with ^D and suchlike
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "Open the pod bay doors, HAL.") {
                // A little bit of fun
                std::cout << "I'm sorry, Dave. I'm afraid I can't do that.";
                std::cout << std::endl << std::endl;
            } else if(choice == "Hello, Joshua.") {
                // Some more fun
                std::cout << "A strange game. The only winning move is";
                std::cout << " not to play." << std::endl;
                std::cout << "How about a nice game of chess?";
                std::cout << std::endl << std::endl;
            } else if(choice == "1") {
                // Actual choices?!
                return MENU_RUN_MAIN_TASK;
            } else if(choice == "2") {
                // Call the sub-menu. If it returned non-back, cascade the
                // return back up. Otherwise, the main menu will reprint.
                MenuChoice option = Menu::self_tests();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "q") {
                return MENU_QUIT;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }    
    }

    /**
     * The Self-Tests submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::self_tests()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select a self test option:" << std::endl;
            std::cout << "1) Run All Self Tests" << std::endl;
            std::cout << "2) Motor Control Tests" << std::endl;
            std::cout << "3) Line Following Test" << std::endl;
            std::cout << "4) Navigation Test" << std::endl;
            std::cout << "5) Clamp Control Tests" << std::endl;
            std::cout << "6) Sensor Tests" << std::endl;
            std::cout << "7) Output Tests" << std::endl;
            std::cout << "8) Task Tests" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";
            
            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_RUN_ALL_SELF_TESTS;
            } else if(choice == "2") {
                MenuChoice option = Menu::motor_control();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "3") {
                return MENU_LINE_FOLLOWING_TEST;
            } else if(choice == "4") {
                MenuChoice option = Menu::navigation();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "5") {
                MenuChoice option = Menu::clamp_control();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "6") {
                MenuChoice option = Menu::sensor_tests();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "7") {
                MenuChoice option = Menu::output_tests();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "8") {
                MenuChoice option = Menu::task_tests();
                if(option != MENU_BACK)
                    return option;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The motor control submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::motor_control()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select a motor control test:" << std::endl;
            std::cout << "1) Drive Forward" << std::endl;
            std::cout << "2) Drive Backward" << std::endl;
            std::cout << "3) Turn Left" << std::endl;
            std::cout << "4) Turn Right" << std::endl;
            std::cout << "5) Steer Left" << std::endl;
            std::cout << "6) Steer Right" << std::endl;
            std::cout << "7) Stop" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_DRIVE_FORWARD;
            } else if(choice == "2") {
                return MENU_DRIVE_BACKWARD;
            } else if(choice == "3") {
                return MENU_TURN_LEFT;
            } else if(choice == "4") {
                return MENU_TURN_RIGHT;
            } else if(choice == "5") {
                return MENU_STEER_LEFT;
            } else if(choice == "6") {
                return MENU_STEER_RIGHT;
            } else if(choice == "7") {
                return MENU_STOP;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The navigation submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::navigation()
    {
        // See get_choice for comments, code is indentical in form.
        while(true) {
            std::cout << "Select a navigation test:" << std::endl;
            std::cout << "1) Navigate to a specific node" << std::endl;
            std::cout << "2) Navigate to a bobbin" << std::endl;
            std::cout << "3) Navigate to a box for drop" << std::endl;
            std::cout << "4) Navigate to a box for pickup" << std::endl;
            std::cout << "5) Navigate to the delivery zone" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_NAVIGATION_TEST;
            } else if(choice == "2") {
                return MENU_NAVIGATE_TO_BOBBIN;
            } else if(choice == "3") {
                return MENU_NAVIGATE_TO_BOX_FOR_DROP;
            } else if(choice == "4") {
                return MENU_NAVIGATE_TO_BOX_FOR_PICKUP;
            } else if(choice == "5") {
                return MENU_NAVIGATE_TO_DELIVERY;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The clamp control submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::clamp_control()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select a clamp control test:" << std::endl;
            std::cout << "1) Analyse Sensors" << std::endl;
            std::cout << "2) Pick up and put down box/bobbin" << std::endl;
            std::cout << "3) Check for bobbin presence" << std::endl;
            std::cout << "4) Check for box presence" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_ANALYSE;
            } else if(choice == "2") {
                return MENU_CLAMP_TESTS;
            } else if(choice == "3") {
                return MENU_BOBBIN_PRESENT;
            } else if(choice == "4") {
                return MENU_BOX_PRESENT;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The sensor tests submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::sensor_tests()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select a sensor test:" << std::endl;
            std::cout << "1) Line Sensors" << std::endl;
            std::cout << "2) Switches" << std::endl;
            std::cout << "3) LDRs" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_LINE_SENSORS;
            } else if(choice == "2") {
                return MENU_SWITCHES;
            } else if(choice == "3") {
                return MENU_LDRS;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The output tests submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::output_tests()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select an output test:" << std::endl;
            std::cout << "1) Indicator LEDs" << std::endl;
            std::cout << "2) Actuators" << std::endl;
            std::cout << "3) Colour sensing LEDs" << std::endl;
            std::cout << "4) Bad bobbin LEDs" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_INDICATOR_LEDS;
            } else if(choice == "2") {
                return MENU_ACTUATORS;
            } else if(choice == "3") {
                return MENU_COLOUR_SENSOR_LEDS;
            } else if(choice == "4") {
                return MENU_BAD_BOBBIN_LED;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

    /**
     * The task tests submenu.
     * \returns A MenuChoice to indicate the user's selection.
     */
    MenuChoice Menu::task_tests()
    {
        // See get_choice for comments, code is identical in form.
        while(true) {
            std::cout << "Select a task test:" << std::endl;
            std::cout << "1) Delivery" << std::endl;
            std::cout << "b) Back" << std::endl;
            std::cout << std::endl << "> ";

            std::string choice;
            std::getline(std::cin, choice);
            if(std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore();
                std::cout << "Invalid selection." << std::endl << std::endl;
            } else if(choice == "1") {
                return MENU_DELIVERY;
            } else if(choice == "b") {
                return MENU_BACK;
            } else {
                std::cout << "Invalid selection." << std::endl << std::endl;
            }
        }
    }

}
