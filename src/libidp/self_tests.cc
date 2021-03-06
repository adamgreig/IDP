// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// self_tests.cc
// Self Tests class implementation

// Use unistd.h for sleep functionality
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "self_tests.h"
#include "hal.h"
#include "mission_supervisor.h"
#include "navigation.h"
#include "line_following.h"
#include "clamp_control.h"

// Debug functionality
#define MODULE_NAME "SelfTests"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {
    /**
     * Constuct a SelfTests instance
     * Completely seperate to mission supervisor and initialises own
     * link to robot, with its own HAL instance
     * \param robot Which robot to link to, or 0 if embedded
     */
    SelfTests::SelfTests(int robot = 0): _robot(robot), _hal(0)
    {
        TRACE("SelfTests("<<robot<<")");
        INFO("Initialising SelfTests");
        this->_hal = new HardwareAbstractionLayer(robot);
    }

    /**
     * Destruct the SelfTests, deleting the HAL
     */
    SelfTests::~SelfTests()
    {
        TRACE("~SelfTests()");
        if(this->_hal)
            delete this->_hal;
    }

    /**
     * Drive the robot forwards for a moment
     */
    void SelfTests::drive_forward()
    {
        TRACE("drive_forward()");
        INFO("Driving forwards");
        this->_hal->motors_forward(127);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }

    /**
     * Drive the robot backwards for a moment
     */
    void SelfTests::drive_backward()
    {
        TRACE("drive_backwardi()");
        INFO("Driving backwards");
        this->_hal->motors_backward(127);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }

    /**
     * Stop all of the robot's motors
     */
    void SelfTests::stop()
    {
        TRACE("stop()");
        INFO("Stopping all motors");
        this->_hal->motors_stop();
    }

    /**
     * Drive motors in opposite directions to turn the robot left on
     * the spot
     */
    void SelfTests::turn_left()
    {
        TRACE("turn_left()");
        INFO("Turning left, both wheels driven");
        this->_hal->motor_left_backward(64);
        this->_hal->motor_right_forward(64);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }

    /**
     * Drive motors in opposite directions to turn the robot right on
     * the spot
     */
    void SelfTests::turn_right()
    {
        TRACE("turn_right()");
        INFO("Turning right, both wheels driven");
        this->_hal->motor_left_forward(64);
        this->_hal->motor_right_backward(64);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }
    
    /**
     * Drive forwards for a moment whilst reducing the speed of the 
     * left motor relative to the right to steer left
     */
    void SelfTests::steer_left()
    {
        TRACE("steer_left()");
        INFO("Turning left, only left wheel driven");
        this->_hal->motor_right_forward(127);
        this->_hal->motor_left_forward(0);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }

    /**
     * Drive forwards for a moment whilst reducing the speed of the
     * right motor relative to the left to steer right
     */
    void SelfTests::steer_right()
    {
        TRACE("steer_right()");
        INFO("Turning right, only left wheel driven");
        this->_hal->motor_left_forward(127);
        this->_hal->motor_right_forward(0);
        usleep(1000000);
        this->_hal->motors_stop();
        INFO("Finished driving.");
    }

    /**
     * Display the status (LINE or NO_LINE) of each of the four IR
     * line following sensors
     */
    void SelfTests::line_sensors()
    {
        TRACE("line_sensors()");
        INFO("Testing line sensors");

        this->_hal->clear_status_register();
        LineSensors sensors = this->_hal->line_following_sensors();
        std::cout << "Sensors: ";
        
        if(sensors.outer_left == LINE)
            std::cout << "line, ";
        else
            std::cout << "no line, ";

        if(sensors.line_left == LINE)
            std::cout << "line, ";
        else
            std::cout << "no line, ";

        if(sensors.line_right == LINE)
            std::cout << "line, ";
        else
            std::cout << "no line, ";

        if(sensors.outer_right == LINE)
            std::cout << "line";
        else
            std::cout << "no line";

        std::cout << std::endl;
    }

    /**
     * Display the state of each of the two microswitches
     */
    void SelfTests::microswitches()
    {
        TRACE("microswitches()");
        INFO("Testing microswitches");
        bool reset = this->_hal->reset_switch();
        bool grabber = this->_hal->grabber_switch();

        std::cout << "Reset switch: " << reset << std::endl;
        std::cout << "Grabber switch: " << grabber << std::endl;
    }
    
    /**
     * Display the current ADC read from the light dependent resistor
     */
    void SelfTests::LDRs()
    {
        TRACE("LDRs()");
        unsigned short int reading;

        // Read without LEDs
        this->_hal->colour_LED(false);
        this->_hal->bad_bobbin_LED(false);
        reading = this->_hal->colour_ldr();
        std::cout << "* No lights, colour: " << reading << std::endl;
        reading = this->_hal->bad_bobbin_ldr();
        std::cout << "* No lights, badness: " << reading << std::endl;

        // Again with colour LED
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(false);
        reading = this->_hal->colour_ldr();
        std::cout << "* Colour LED, colour: " << reading << std::endl;
        reading = this->_hal->bad_bobbin_ldr();
        std::cout << "* Colour LED, badness: " << reading << std::endl;

        // Now with the badness LED
        this->_hal->colour_LED(false);
        this->_hal->bad_bobbin_LED(true);
        reading = this->_hal->colour_ldr();
        std::cout << "* Badness LED, colour: " << reading << std::endl;
        reading = this->_hal->bad_bobbin_ldr();
        std::cout << "* Badness LED, badness: " << reading << std::endl;

        // And finally with both
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(true);
        reading = this->_hal->colour_ldr();
        std::cout << "* Both LEDs, colour: " << reading << std::endl;
        reading = this->_hal->bad_bobbin_ldr();
        std::cout << "* Both LEDs, badness: " << reading << std::endl;

        this->_hal->colour_LED(false);
        this->_hal->bad_bobbin_LED(false);
        std::cout << "Done." << std::endl;
    }

    /**
     * Fire each of the actuators in turn
     */
    void SelfTests::actuators()
    {
        TRACE("actuators()");
        INFO("Testing actuators");
        std::cout << "Closing jaw..." << std::endl;
        this->_hal->grabber_jaw(true);
        usleep(1000000);
        std::cout << "Opening jaw..." << std::endl;
        this->_hal->grabber_jaw(false);
        usleep(1000000);
        std::cout << "Raising arm..." << std::endl;
        this->_hal->grabber_lift(true);
        usleep(1000000);
        std::cout << "Lowing arm..." << std::endl;
        this->_hal->grabber_lift(false);
    }

    /**
     * Follow a line until a junction
     */
    void SelfTests::line_following()
    {
        TRACE("line_following()");
        INFO("Testing line following");

        LineFollowing lf(this->_hal);
        LineFollowingStatus status;

        lf.set_speed(127);
        
        INFO("following a line");
        do {
            status = lf.follow_line();
        } while(status == ACTION_IN_PROGRESS);
    }

    /**
     * Use the actuators to pick up an object before placing it back down
     * again
     */
    void SelfTests::clamp_control()
    {
        TRACE("clamp_control()");
        INFO("Testing clamp control");

        ClampControl cc(this->_hal);
        cc.pick_up();

        std::cout << "Picked up, press enter to put down again." << std::endl;

        std::getchar();
        cc.put_down();

        std::cout << "Done." << std::endl;
    }

    /**
     * Analyse the colour of the bobbin that is currently being held
     * in the clamp
     */
    void SelfTests::bobbin_analyse()
    {
        TRACE("bobbin_analyse()");

        ClampControl cc(this->_hal);
        

        std::cout << "Position a bobbin inside the clamp and press enter.";
        std::cout << std::endl;

        std::getchar();

        BobbinColour colour = cc.colour();
        std::cout << "Read colour as " << BobbinColourStrings[colour];
        std::cout << std::endl;

        BobbinBadness bad = cc.badness();
        std::cout << "Read badness as " << BobbinBadnessStrings[bad];
        std::cout << std::endl << "Done." << std::endl;

    }

    /**
     * Analyse the colour of the bobbin in a box
     */
    void SelfTests::box_analyse()
    {
        TRACE("box_analyse()");

        ClampControl cc(this->_hal);

        std::cout << "Position a box inside the clamp and press enter.";
        std::cout << std::endl;

        std::getchar();

        
        BobbinColour colour = cc.box_colour();
        std::cout << "Read colour as " << BobbinColourStrings[colour];
        std::cout << std::endl;

    }

    /**
     * See if a bobbin is under the clamp
     */
    void SelfTests::bobbin_present()
    {
        TRACE("bobbin_present()");

        ClampControl cc(this->_hal);
        
        std::cout << "Make sure the clamp is not over a bobbin and press";
        std::cout << " enter." << std::endl;

        std::getchar();
        cc.open_jaw();
        cc.lower_arm();

        std::cout << "Now press enter to test for a bobbin." << std::endl;

        std::getchar();

        for(;;) {

            bool present = cc.bobbin_present();

            if(present)
                std::cout << "Bobbin found!" << std::endl;
            else
                std::cout << "No bobbin found." << std::endl;
        }
    }

    /**
     * See if a box is under the clamp
     */
    void SelfTests::box_present()
    {
        TRACE("box_present()");
        ClampControl cc(this->_hal);
        std::cout << "Testing for box...";

        std::cout << "Make sure the clamp is not over a box and press";
        std::cout << " enter." << std::endl;

        std::getchar();
        cc.open_jaw();
        cc.lower_arm();

        std::cout << "Now press enter to test for a box." << std::endl;

        std::getchar();

        for(;;) {
            bool present = cc.box_present();
            if(present)
                std::cout << "Box found!" << std::endl;
            else
                std::cout << "No box found." << std::endl;
        }
    }

    /**
     * Select a source and destination and then navigate to the
     * destination assuming we are starting at the source
     */
    void SelfTests::navigate()
    {
        TRACE("navigate()");
        INFO("Testing navigation");

        int input_node;

        std::cout << "Enter FROM node: ";
        std::cin >> input_node;
        NavigationNode from = static_cast<NavigationNode>(input_node - 1);
        std::cout << "FROM is " << NavigationNodeStrings[from] << std::endl;

        std::cout << "Enter TO node: ";
        std::cin >> input_node;
        NavigationNode to = static_cast<NavigationNode>(input_node - 1);
        std::cout << "TO is " << NavigationNodeStrings[to] << std::endl;

        std::cout << "Enter TARGET node: ";
        std::cin >> input_node;
        NavigationNode target = static_cast<NavigationNode>(input_node - 1);
        std::cout << "TARGET is " << NavigationNodeStrings[target];
        std::cout << std::endl;

        Navigation nav(this->_hal, from, to);
        NavigationStatus status;
        do {
            status = nav.go_node(target);
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();
        return;
    }
    
    /**
     * Drive to a bobbin and stop.
     */
    void SelfTests::navigate_to_bobbin()
    {
        TRACE("navigate_to_bobbin()");
        INFO("Navigating to a bobbin");

        std::cout << "Please position Fluffy in the start box facing";
        std::cout << " the bobbins, then press enter." << std::endl;

        std::getchar();

        Navigation nav(this->_hal, NODE7, NODE8);
        NavigationStatus status;
        do {
            status = nav.find_bobbin();
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();

        INFO("Found bobbin. Press 'p' to pick up or anything"
            << " else to quit");

        char in = std::getchar();
        if(in == 'p') {
            ClampControl cc(this->_hal);
            cc.pick_up();
        }

        return;
    }
    
    /**
     * Locate a box and stop.
     */
    void SelfTests::navigate_to_box_for_pickup()
    {
        TRACE("navigate_to_box_for_pickup()");
        INFO("Running navigate to box for pickup test");

        std::cout << "Which box would you like to pick up? (1 or 2)";
        std::cout << std::endl << "> ";

        int box_number;
        std::cin >> box_number;
        Box box = static_cast<Box>(box_number - 1);

        std::cout << "Going to box " << BoxStrings[box] << "." << std::endl;

        std::cout << "Please position Fluffy on the line between nodes";
        std::cout << " 7 and 8 (in the start box), then press enter.";
        std::cout << std::endl;

        std::getchar();
        std::getchar();

        Navigation nav(this->_hal, NODE7, NODE8);
        NavigationStatus status;
        do {
            status = nav.find_box_for_pickup(box);
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();

        INFO("Found box for pickup. Press 'p' to pick up or anything"
            << " else to quit");

        char in = std::getchar();
        if(in == 'p') {
            ClampControl cc(this->_hal);
            cc.pick_up();
        }

        return;
    }

    /**
     * Locate a box and stop.
     */
    void SelfTests::navigate_to_box_for_drop()
    {
        TRACE("navigate_to_box_for_drop()");
        INFO("Running navigate to box for drop test");

        std::cout << "Which box would you like to deliver to? (1 or 2)";
        std::cout << std::endl << "> ";

        int box_number;
        std::cin >> box_number;
        Box box = static_cast<Box>(box_number - 1);

        std::cout << "Going to box " << BoxStrings[box] << "." << std::endl;

        std::cout << "Please position Fluffy on the line between nodes";
        std::cout << " 9 and 8 (facing the start box), then press enter.";
        std::cout << std::endl;

        std::getchar();
        std::getchar();

        Navigation nav(this->_hal, NODE9, NODE8);
        NavigationStatus status;
        do {
            status = nav.find_box_for_drop(box);
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();

        INFO("Found box for dropoff. Press 'd' to drop or anything"
            << " else to quit");

        char in = std::getchar();
        if(in == 'd') {
            ClampControl cc(this->_hal);
            cc.put_down();
        }

        return;
    }

    /**
     * Drive to the delivery zone and stop.
     */
    void SelfTests::navigate_to_delivery()
    {
        TRACE("navigate_to_delivery()");
        INFO("Running navigate to delivery test");

        std::cout << "Please place Fluffy between junctions 4 and 3 (facing";
        std::cout << " the delivery area) and press enter." << std::endl;

        std::getchar();

        Navigation nav(this->_hal, NODE4, NODE3);
        NavigationStatus status;
        do {
            status = nav.go_to_delivery();
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();

        std::cout << "Now at delivery. Press enter to return to the path.";
        std::cout << std::endl;

        std::getchar();

        do {
            status = nav.finished_delivery();
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();
        return;
    }

    /**
     * Drive to the delivery zone and drop something off then return.
     */
    void SelfTests::delivery()
    {
        TRACE("delivery()");
        INFO("Running delivery test");

        std::cout << "Please place Fluffy between junctions 4 and 3 (facing";
        std::cout << " the delivery area) and press enter." << std::endl;

        std::getchar();

        std::cout << "Please place a box under Fluffy's jaws and press enter.";
        std::cout << std::endl;

        std::getchar();

        ClampControl cc(this->_hal);
        cc.pick_up();

        Navigation nav(this->_hal, NODE4, NODE3);
        NavigationStatus status;
        do {
            status = nav.go_to_delivery();
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();

        cc.put_down();

        do {
            status = nav.finished_delivery();
        } while(status == NAVIGATION_ENROUTE);

        do {
            status = nav.go_node(NODE4);
        } while(status == NAVIGATION_ENROUTE);

        this->_hal->motors_stop();
        return;
    }
    
    /**
     * Drive slowly looking for an object in range for pickup, 
     * then position self ready to clamp said object
     */
    void SelfTests::position()
    {
        TRACE("position()");
    }

    /**
     * Turn on each of the status LEDs (used for indicating bobbin
     * colour) in turn
     */
    void SelfTests::indicator_LEDs()
    {
        TRACE("indicator_LEDs()");
        this->_hal->indication_LEDs(false, false, true);
        usleep(500000);
        this->_hal->indication_LEDs(false, true, false);
        usleep(500000);
        this->_hal->indication_LEDs(false, true, true);
        usleep(500000);
        this->_hal->indication_LEDs(true, false, false);
        usleep(500000);
        this->_hal->indication_LEDs(true, false, true);
        usleep(500000);
        this->_hal->indication_LEDs(true, true, false);
        usleep(500000);
        this->_hal->indication_LEDs(true, true, true);
        usleep(500000);
        this->_hal->indication_LEDs(false, false, false);
        usleep(500000);
    }

    /**
     * Turn on each of the coloured LEDs used for colour detection
     * in turn
     */
    void SelfTests::colour_sensor_LEDs()
    {
        TRACE("colour_sensor_LEDs()");
        this->_hal->colour_LED(true);
        usleep(100000);
        this->_hal->colour_LED(false);
    }

    /**
     * Turn on the LED used for detecting bad bobbins
     */
    void SelfTests::bad_bobbin_LED()
    {
        TRACE("bad_bobbin_LED()");
        this->_hal->bad_bobbin_LED(true);
        usleep(500000);
        this->_hal->bad_bobbin_LED(false);
        usleep(500000);
    }

    /**
     * Read a lot of levels from the LDRs to store in a calibration file
     */
    void SelfTests::calibrate()
    {
        TRACE("calibrate()");

        INFO("Taking zero-level readings");
        ClampControl cc(this->_hal);
        
        std::cout << "Please ensure the arm is down and press enter."
            << std::endl;
        cc.lower_arm();
        std::getchar();

        std::cout << "Please ensure the jaw is closed and press enter."
            << std::endl;
        cc.close_jaw();
        std::getchar();
        this->_hal->colour_LED(true);
        this->_hal->bad_bobbin_LED(false);
        short int colour_light_closed_zero = cc.average_colour_ldr(15);
        
        std::cout << "Please ensure the jaw is open and press enter."
            << std::endl;
        cc.open_jaw();
        std::getchar();
        short int colour_light_zero = cc.average_colour_ldr(15);
        this->_hal->colour_LED(false);
        short int badness_dark_zero = cc.average_bad_ldr(15);
        this->_hal->bad_bobbin_LED(true);
        short int badness_light_zero = cc.average_bad_ldr(15);
        this->_hal->bad_bobbin_LED(false);

        std::cout << "Place an empty box in front of the sensor and press"
            << " enter." << std::endl;
        std::getchar();

        this->_hal->colour_LED(true);
        short int colour_light_box_zero = cc.average_colour_ldr(15);
        this->_hal->colour_LED(false);
        
        std::cout << "Base levels taken: "
            << "colour_light_closed_zero " << colour_light_closed_zero
            << "colour_light_zero " << colour_light_zero
            << "badness_light_zero " << badness_light_zero
            << "badness_dark_zero " << badness_dark_zero
            << "colour_light_box_zero " << colour_light_box_zero << std::endl;

        std::cout << "Place a red bobbin in the box in the jaw and press enter"
            << std::endl;
        std::getchar();
        this->_hal->colour_LED(true);
        short int red_box_level = cc.average_colour_ldr(15);
        red_box_level -= colour_light_box_zero;
        std::cout << "Read " << red_box_level << " for red box level, enter"
            << " the value you want used: ";
        std::cin >> red_box_level;
        std::cout << "Using " << red_box_level << std::endl;

        std::cout << "Place a green bobbin in the box in the jaw and press "
            << "enter" << std::endl;
        std::getchar();
        std::getchar();
        short int green_box_level = cc.average_colour_ldr(15);
        green_box_level -= colour_light_box_zero;
        std::cout << "Read " << green_box_level << " for green box level, enter"
            << " the value you want used: ";
        std::cin >> green_box_level;
        std::cout << "Using " << green_box_level << std::endl;

        std::cout << "Please ensure the jaw is closed and press enter."
            << std::endl;
        cc.close_jaw();
        std::getchar();

        std::cout << "Please place a red bobbin in the jaw (on the rack) and"
            << " press enter." << std::endl;
        std::getchar();
        this->_hal->colour_LED(true);
        short int red_rack_level = cc.average_colour_ldr(15);
        red_rack_level -= colour_light_closed_zero;
        std::cout << "Read " << red_rack_level << " for red rack level, enter"
            << " the value you want used: ";
        std::cin >> red_rack_level;
        std::cout << "Using " << red_rack_level << std::endl;

        std::cout << "Place a green bobbin in the jaw (on the rack) and press"
            << " enter." << std::endl;
        std::getchar();
        std::getchar();
        short int green_rack_level = cc.average_colour_ldr(15);
        green_rack_level -= colour_light_closed_zero;
        std::cout << "Read " << green_rack_level << " for green rack level, "
            << "enter the value you want used: ";
        std::cin >> green_rack_level;
        std::cout << "Using " << green_rack_level << std::endl;
        this->_hal->colour_LED(false);

        std::cout << "Please ensure the jaw is open and press enter."
            << std::endl;
        cc.open_jaw();
        std::getchar();

        std::cout << "Place a box under the jaw and press enter."
            << std::endl;
        std::getchar();
        std::getchar();
        this->_hal->bad_bobbin_LED(true);
        short int box_present_level = cc.average_bad_ldr(15);
        box_present_level -= badness_light_zero;
        std::cout << "Read " << box_present_level << " for box present level, "
            << "enter the value you want used: ";
        std::cin >> box_present_level;
        std::cout << "Using " << box_present_level << std::endl;
        this->_hal->bad_bobbin_LED(false);

        std::cout << "Please place a white bobbin under the open jaw on the"
            << " rack and press enter." << std::endl;
        std::getchar();
        std::getchar();
        short int white_present_level = cc.average_bad_ldr(15);
        white_present_level -= badness_dark_zero;
        std::cout << "Read " << white_present_level << " for white present"
            << " level, enter the value you want used: ";
        std::cin >> white_present_level;
        std::cout << "Using " << white_present_level << std::endl;
        
        std::cout << "Please place a green bobbin under the open jaw on the"
            << " rack and press enter" << std::endl;
        std::getchar();
        std::getchar();
        this->_hal->colour_LED(true);
        short int coloured_present_level = cc.average_colour_ldr(15);
        coloured_present_level -= colour_light_zero;
        std::cout << "Read " << coloured_present_level << " for green present,"
            << " place a red bobbin under the sensor and press enter."
            << std::endl;
        std::getchar();
        std::getchar();
        coloured_present_level = cc.average_colour_ldr(15);
        coloured_present_level -= colour_light_zero;
        std::cout << "Read " << coloured_present_level << " for red present, "
            << "enter the value you want used: ";
        std::cin >> coloured_present_level;
        std::cout << "Using " << coloured_present_level;

        std::cout << "Writing file" << std::endl;
        
        std::ofstream f("levelsfile");
        f
            << colour_light_closed_zero << std::endl
            << colour_light_zero << std::endl
            << badness_light_zero << std::endl
            << badness_dark_zero << std::endl
            << colour_light_box_zero << std::endl
            << red_box_level << std::endl
            << green_box_level << std::endl
            << red_rack_level << std::endl
            << green_rack_level << std::endl
            << box_present_level << std::endl
            << white_present_level << std::endl
            << coloured_present_level << std::endl;
        f.close();

        std::cout << "Wrote file" << std::endl;

    }
}
