// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// self_tests.cc
// Self Tests class implementation


#include "self_tests.h"
#include "hal.h"
#include "mission_supervisor.h"
#include "navigation.h"
#include "line_following.h"

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
    }

    /**
     * Drive the robot backwards for a moment
     */
    void SelfTests::drive_backward()
    {
        TRACE("drive_backwardi()");
        INFO("Driving backwards");
        this->_hal->motors_backward(127);
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
        this->_hal->motor_left_forward(64);
        this->_hal->motor_right_backward(64);
    }

    /**
     * Drive motors in opposite directions to turn the robot right on
     * the spot
     */
    void SelfTests::turn_right()
    {
        TRACE("turn_right()");
        INFO("Turning right, both wheels driven");
        this->_hal->motor_right_forward(64);
        this->_hal->motor_left_backward(64);
    }
    
    /**
     * Drive forwards for a moment whilst reducing the speed of the 
     * left motor relative to the right to steer left
     */
    void SelfTests::steer_left()
    {
        TRACE("steer_left()");
        INFO("Turning left, only left wheel driven");
        this->_hal->motor_left_forward(127);
        this->_hal->motor_right_forward(0);
    }

    /**
     * Drive forwards for a moment whilst reducing the speed of the
     * right motor relative to the left to steer right
     */
    void SelfTests::steer_right()
    {
        TRACE("steer_right()");
        INFO("Turning right, only left wheel driven");
        this->_hal->motor_right_forward(127);
        this->_hal->motor_left_forward(0);
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
            std::cout << "line             \r";
        else
            std::cout << "no line          \r";
    }

    /**
     * Display the state of each of the two microswitches
     */
    void SelfTests::microswitches()
    {
        TRACE("microswitches()");
    }
    
    /**
     * Display the current ADC read from the light dependent resistor
     */
    void SelfTests::LDRs()
    {
        TRACE("LDRs()");
    }

    /**
     * Fire each of the actuators in turn
     */
    void SelfTests::actuators()
    {
        TRACE("actuators()");
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
    }

    /**
     * Analyse the colour of the bobbin that is currently being held
     * in the clamp
     */
    void SelfTests::bobbin_analyse()
    {
        TRACE("bobbin_analyse()");
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
    void SelfTests::status_LEDs()
    {
        TRACE("status_LEDs()");
    }

    /**
     * Turn on each of the coloured LEDs used for colour detection
     * in turn
     */
    void SelfTests::colour_sensor_LEDs()
    {
        TRACE("colour_sensor_LEDs()");
    }

    /**
     * Turn on the LED used for detecting bad bobbins
     */
    void SelfTests::badness_LED()
    {
        TRACE("badness_LED()");
    }
}
