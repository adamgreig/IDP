// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// main.cc
// Program entry point

#include <iostream>
#include <string>
#include <signal.h>
#include <cstdlib>
#include <libidp/libidp.h>

#include "menu.h"

/**
 * The robot number, or 0 if running embedded.
 */
static const int ROBOT = 39;

/**
 * Global reference to our Mission Supervisor so we can use it inside
 * terminate()
 */
static IDP::MissionSupervisor* missup = 0;

/*
 * Global reference to SelfTests so they can be stopped inside terminate()
 */
static IDP::SelfTests* tests = 0;

/**
 * Handle program termination cleanly.
 * \param param Signal number (typically SIGINT)
 */
void terminate(int param)
{
    // Check we have a SIGINT
    if(param != SIGINT) {
        return;
    }

    // Stop the robot if either missup or tests was constructed
    if(missup) {
        missup->stop();
    }

    if(tests) {
        tests->stop();
    }

    // Exit OK
    exit(1);
}

/**
 * Code entry point and main loop.
 */
int main()
{
    // Set up ctrl-c catching
    struct sigaction sigint_handler;
    sigint_handler.sa_handler = terminate;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;
    sigaction(SIGINT, &sigint_handler, NULL);

    // Menu loop
    for(;;)
    {
        // Get user action choice
        IDP::MenuChoice choice = IDP::Menu::get_choice();
        
        // Act appropriately. Unfortunately no way besides a large if/else if
        // statement seems particularly appropriate (in particular a switch
        // will not work and in any case is hardly any neater).
        if(choice == IDP::MENU_QUIT) {
            return 0;
        } else if(choice == IDP::MENU_RUN_MAIN_TASK) {
            // 1. check for existing state files
            // 2. see if we should load them
            // 3. initialise a missionsupervisor appropriately
            missup = new IDP::MissionSupervisor(ROBOT);
            // 4. run it
            missup->run_task();
        } else {
            // One of the tests was selected, so initialise tests
            tests = new IDP::SelfTests(ROBOT);
            if(choice == IDP::MENU_RUN_ALL_SELF_TESTS) {
                // this should be interesting, work out a good way to do this
                // interatively in a bit
            } else if(choice == IDP::MENU_LINE_FOLLOWING_TEST) {
                tests->line_following();
            } else if(choice == IDP::MENU_NAVIGATION_TEST) {
                tests->navigate();
            } else if(choice == IDP::MENU_NAVIGATE_TO_BOX) {
                tests->navigate_to_box();
            } else if(choice == IDP::MENU_NAVIGATE_TO_BOBBIN) {
                tests->navigate_to_bobbin();
            } else if(choice == IDP::MENU_NAVIGATE_TO_DELIVERY) {
                tests->navigate_to_delivery();
            } else if(choice == IDP::MENU_DELIVERY) {
                tests->delivery();
            } else if(choice == IDP::MENU_DRIVE_FORWARD) {
                tests->drive_forward();
            } else if(choice == IDP::MENU_DRIVE_BACKWARD) {
                tests->drive_backward();
            } else if(choice == IDP::MENU_TURN_LEFT) {
                tests->turn_left();
            } else if(choice == IDP::MENU_TURN_RIGHT) {
                tests->turn_right();
            } else if(choice == IDP::MENU_STEER_LEFT) {
                tests->steer_left();
            } else if(choice == IDP::MENU_STEER_RIGHT) {
                tests->steer_right();
            } else if(choice == IDP::MENU_STOP) {
                tests->stop();
            } else if(choice == IDP::MENU_ANALYSE) {
                tests->bobbin_analyse();
            } else if(choice == IDP::MENU_CLAMP_TESTS) {
                tests->clamp_control();
            } else if(choice == IDP::MENU_BOBBIN_PRESENT) {
                tests->bobbin_present();
            } else if(choice == IDP::MENU_BOX_PRESENT) {
                tests->box_present();
            } else if(choice == IDP::MENU_LINE_SENSORS) {
                tests->line_sensors();
            } else if(choice == IDP::MENU_SWITCHES) {
                tests->microswitches();
            } else if(choice == IDP::MENU_LDRS) {
                tests->LDRs();
            } else if(choice == IDP::MENU_INDICATOR_LEDS) {
                tests->indicator_LEDs();
            } else if(choice == IDP::MENU_ACTUATORS) {
                tests->actuators();
            } else if(choice == IDP::MENU_COLOUR_SENSOR_LEDS) {
                tests->colour_sensor_LEDs();
            } else if(choice == IDP::MENU_BAD_BOBBIN_LED) {
                tests->bad_bobbin_LED();
                // TODO: position test
            } else {
                std::cout << "Invalid selection received from menu, quitting.";
                std::cout << std::endl;
                return 0;
            }

            // Clear up tests
            delete tests;
        }
    }

    return 0;
}

