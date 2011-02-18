// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// main.cc
// Program entry point

#include <iostream>
#include <fstream>
#include <string>
#include <signal.h>
#include <cstdlib>
#include <libidp/libidp.h>

#include "menu.h"

/**
 * The robot number, or 0 if running embedded.
 */
#ifdef RUNNING_EMBEDDED
static const int ROBOT = 0;
#else
static const int ROBOT = 39;
#endif

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

    // Save state from missup
    if(missup) {
        std::ofstream f("statefile");
        bool red, green, white, box1;
        missup->export_state(red, green, white, box1);
        f << red << std::endl << green << std::endl << white << std::endl;
        f << box1 << std::endl;
        f.close();
    }

    // Exit OK
    exit(1);
}

/**
 * A big if statement to run the chosen test.
 * \param choice The user's menu selection
 */
void run_test(MenuChoice choice)
{
    // One of the tests was selected, so initialise tests
    tests = new IDP::SelfTests(ROBOT);
    if(choice == IDP::MENU_RUN_ALL_SELF_TESTS) {
        // TODO: this complete self test routine
    } else if(choice == IDP::MENU_LINE_FOLLOWING_TEST) {
        tests->line_following();
    } else if(choice == IDP::MENU_NAVIGATION_TEST) {
        tests->navigate();
    } else if(choice == IDP::MENU_NAVIGATE_TO_BOX_FOR_DROP) {
        tests->navigate_to_box_for_drop();
    } else if(choice == IDP::MENU_NAVIGATE_TO_BOX_FOR_PICKUP) {
        tests->navigate_to_box_for_pickup();
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
        return;
    }

    // Clear up tests
    delete tests;
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
            // Make a MissionSupervisor
            missup = new IDP::MissionSupervisor(ROBOT);

            // Check if we should load a state file
            std::cout << "Load state file? (y/N) > ";
            std::string in;
            std::cin >> in;
            if(in == "y") {
                std::ifstream f("statefile");
                bool red, green, white, box1;
                f >> red >> green >> white >> box1;
                missup->load_state(red, green, white, box1);
            }

            // Run the task
            missup->run_task();
        } else {
            run_test(choice);
        }
    }

    return 0;
}

