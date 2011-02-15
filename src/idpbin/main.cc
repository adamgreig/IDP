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
        // 4. run it
    } else if(choice == IDP::MENU_RUN_ALL_SELF_TESTS) {
        // this should be interesting, work out a good way to do this
        // interatively in a bit
    } else if(choice == IDP::MENU_LINE_FOLLOWING_TEST) {
        tests = new IDP::SelfTests(ROBOT);
        tests->line_following();
    } else if(choice == IDP::MENU_NAVIGATION_TEST) {
        tests = new IDP::SelfTests(ROBOT);
        tests->navigate();
    } else if(choice == IDP::MENU_DRIVE_FORWARD) {
        tests = new IDP::SelfTests(ROBOT);
        tests->drive_forward();
    } else if(choice == IDP::MENU_DRIVE_BACKWARD) {
        tests = new IDP::SelfTests(ROBOT);
        tests->drive_backward();
    } else if(choice == IDP::MENU_TURN_LEFT) {
        tests = new IDP::SelfTests(ROBOT);
        tests->turn_left();
    } else if(choice == IDP::MENU_TURN_RIGHT) {
        tests = new IDP::SelfTests(ROBOT);
        tests->turn_right();
    } else if(choice == IDP::MENU_STEER_LEFT) {
        tests = new IDP::SelfTests(ROBOT);
        tests->steer_left();
    } else if(choice == IDP::MENU_STEER_RIGHT) {
        tests = new IDP::SelfTests(ROBOT);
        tests->steer_right();
    } else if(choice == IDP::MENU_STOP) {
        tests = new IDP::SelfTests(ROBOT);
        tests->stop();
    } else if(choice == IDP::MENU_ANALYSE) {
        tests = new IDP::SelfTests(ROBOT);
        tests->bobbin_analyse();
    } else if(choice == IDP::MENU_PICK_UP) {
        // TODO: this just calls clamp_control! it should really
        // call something to pick something up
        tests = new IDP::SelfTests(ROBOT);
        tests->clamp_control();
    } else if(choice == IDP::MENU_PUT_DOWN) {
        // TODO: see above!
        tests = new IDP::SelfTests(ROBOT);
        tests->clamp_control();
    } else if(choice == IDP::MENU_LINE_SENSORS) {
        tests = new IDP::SelfTests(ROBOT);
        tests->line_sensors();
    } else if(choice == IDP::MENU_SWITCHES) {
        tests = new IDP::SelfTests(ROBOT);
        tests->microswitches();
    } else if(choice == IDP::MENU_LDRS) {
        tests = new IDP::SelfTests(ROBOT);
        tests->LDRs();
    } else if(choice == IDP::MENU_INDICATOR_LEDS) {
        tests = new IDP::SelfTests(ROBOT);
        tests->indicator_LEDs();
    } else if(choice == IDP::MENU_ACTUATORS) {
        tests = new IDP::SelfTests(ROBOT);
        tests->actuators();
    } else if(choice == IDP::MENU_COLOUR_SENSOR_LEDS) {
        tests = new IDP::SelfTests(ROBOT);
        tests->colour_sensor_LEDs();
    } else if(choice == IDP::MENU_BAD_BOBBIN_LED) {
        tests = new IDP::SelfTests(ROBOT);
        tests->bad_bobbin_LED();
        // TODO: position test
    } else {
        std::cout << "Invalid selection received from menu, quitting.";
        std::cout << std::endl;
        return 0;
    }

    // Wait forever (Tests/MissionSupervisor can loop or something)
    // TODO: this shouldn't really wait forever here, ideally
    // we should loop on calling MissionSupervisor as it is in the
    // library and all that.
    for(;;);

    return 0;
}

