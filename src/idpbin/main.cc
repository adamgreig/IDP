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

/**
 * The robot number, or 0 if running embedded.
 */
static const int ROBOT = 39;

/**
 * Menu option consts
 */
static const int MENU_DRIVE_FORWARD = 1;
static const int MENU_DRIVE_BACKWARD = 2;

/**
 * Global reference to our Mission Supervisor so we can
 * use it inside terminate()
 */
static IDP::MissionSupervisor* missup = 0;

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

    // Stop the robot
    if(missup) {
        missup->stop();
    }

    // Exit OK
    exit(1);
}

/**
 * Print the UI menu options and check their selection for validity.
 * \return The user's selection
 */
int menu()
{
    while(true) {
        // Display menu options
        std::cout << "What would you like to do?" << std::endl;
        std::cout << "1) Drive forward" << std::endl;
        std::cout << "2) Drive backward" << std::endl;
        std::cout << std::endl;
        std::cout << "> ";

        std::string choice;
        std::getline(std::cin, choice);

        // If they selected a valid choice, return the constant
        // related to that choice. If they selected an invalid choice,
        // return an error and re-display the menu.
        if(std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Invalid selection." << std::endl << std::endl;
        } else if(choice == "1") {
            return MENU_DRIVE_FORWARD;
        } else if(choice == "2") {
            return MENU_DRIVE_BACKWARD;
        } else if(choice == "Open the pod bay doors, HAL.") {
            std::cout << "I'm sorry, Dave. I'm afraid I can't do that.";
            std::cout << std::endl << std::endl;
        } else if(choice == "Hello, Joshua.") {
            std::cout << "A strange game. The only winning move is";
            std::cout << " not to play." << std::endl;
            std::cout << "How about a nice game of chess?";
            std::cout << std::endl << std::endl;
        } else {
            std::cout << "Invalid selection." << std::endl << std::endl;
        }

    }
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

    // Initialise the robot link
    missup = new IDP::MissionSupervisor(ROBOT);
    
    std::cout << "Welcome to The El Theta's IDP Mission Control.";
    std::cout << std::endl;

    // Get user action choice
    int choice = menu();
    if(choice == MENU_DRIVE_FORWARD) {
        missup->drive_forward();
    } else if(choice == MENU_DRIVE_BACKWARD) {
        missup->drive_backward();
    }

    // Wait forever (MissionSupervisor should take care of things)
    for(;;);

    return 0;
}

