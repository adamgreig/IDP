// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// hal.cc
// Hardware Abstraction Layer implementation

#include "hal.h"

#include <iostream>
#include <cstdlib>

#include <robot_instr.h>

// Debug functionality
#define MODULE_NAME "HAL"
#define TRACE_ENABLED   true
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

// For now, define this to avoid unused-variable errors
#define UNUSED(x) (void)(x)

namespace IDP {

    /**
     * Initialise the HAL class.
     * Establishes the link to the robot.
     */
    HardwareAbstractionLayer::HardwareAbstractionLayer(const int robot=0)
    {
        TRACE("HardwareAbstractionLayer(" << robot << ")");
        INFO("Constructing HAL");

        int status;
        this->rlink = new robot_link;

        // Initialise link
        INFO("Initialising link");
        if(robot == 0) {
            status = this->rlink->initialise();
        } else {
            status = this->rlink->initialise(robot);
        }

        // Check link, exit on failure
        if(!status) {
            ERROR("Error initialising rlink, quitting");
            std::exit(1);
            return;
        }

        // Set motor ramp speed
        DEBUG("Setting motor ramp speed to " << MOTOR_RAMP_TIME);
        this->rlink->command(RAMP_TIME, MOTOR_RAMP_TIME);

        // Initialise the value of the sensor port
        DEBUG("Reading the value of the hardware port");
        this->_port7 = this->rlink->request(READ_PORT_7);

        // Setting PORT0 to all inputs
        this->rlink->command(WRITE_PORT_0, 0xFF);
    }

    /**
     * Drive both motors forwards at a given speed.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_forward(
        const unsigned short int speed)
    {
        TRACE("motors_forward(" << speed << ")");
        DEBUG("Driving motors forward, speed " << speed);

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_OPPOSITE, (1<<7) | speed);
    }

    /**
     * Drive both motors backwards at a given speed.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_backward(
        const unsigned short int speed)
    {
        TRACE("motors_backward(" << speed << ")");
        DEBUG("Driving motors backward, speed " << speed);

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_OPPOSITE, (0<<7) | speed);
    }

    /**
     * Drive the left motor forward at the given speed
     * \param speed The speed at which to drive the motor
     */
    void HardwareAbstractionLayer::motor_left_forward(
        const unsigned short int speed)
    {
        TRACE("motors_turn_right(" << speed << ")");
        DEBUG("Setting left motor forward at speed " << speed);

        if (this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(MOTOR_1_GO, (1<<7) | speed);
    }

    /**
     * Drive the right motor forward at the given speed
     * \param speed The speed at which to drive the motor
     */
    void HardwareAbstractionLayer::motor_right_forward(
        const unsigned short int speed)
    {
        TRACE("motor_right_forward(" << speed << ")");
        DEBUG("Setting right motor forward at speed " << speed);

        if (this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(MOTOR_2_GO, speed);
    }

    /**
     * Drive the left motor backward at the given speed
     * \param speed The speed at which to drive the motor
     */
    void HardwareAbstractionLayer::motor_left_backward(
        const unsigned short int speed)
    {
        TRACE("motor_left_backward(" << speed << ")");
        DEBUG("Setting left motor backward at speed " << speed);

        if (this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(MOTOR_1_GO, speed);
    }

    /**
     * Drive the right motor backward at the given speed
     * \param speed The speed at which to drive the motor
     */
    void HardwareAbstractionLayer::motor_right_backward(
        const unsigned short int speed)
    {
        TRACE("motor_right_backward(" << speed << ")");
        DEBUG("Setting right motor backward at speed " << speed);

        if (this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(MOTOR_2_GO, (1<<7) | speed);
    }

    /**
     * Drive the motors to steer the robot to the left.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_turn_left(
        const unsigned short int speed)
    {
        TRACE("motors_turn_left(" << speed << ")");
        DEBUG("Turning motors left, speed " << speed);

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_SAME, (1<<7) | speed);
    }

    /**
     * Drive the motors to steer the robot to the right.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_turn_right(
        const unsigned short int speed)
    {
        TRACE("motors_turn_right(" << speed << ")");
        DEBUG("Turning motors right, speed " << speed);

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_SAME, (0<<7) | speed);
    }

    /**
     * Stop all motors.
     */
    void HardwareAbstractionLayer::motors_stop()
    {
        TRACE("motors_stop()");
        INFO("Stopping all motors");
        this->rlink->command(MOTOR_1_GO, 0);
        this->rlink->command(MOTOR_2_GO, 0);
        this->rlink->command(MOTOR_3_GO, 0);
        this->rlink->command(MOTOR_4_GO, 0);
    }

    /**
     * Read the I/O port connected to the line following sensors, then
     * return a struct with their current state.
     * \return A LineSensors struct containing the current state of the sensors
     */
    const LineSensors HardwareAbstractionLayer::line_following_sensors() const
    {
        TRACE("line_following_sensors()");
        DEBUG("Reading line following sensors");

        // Get the port values
        int port_values = this->rlink->request(READ_PORT_0);
        LineSensors sensor_values;
        
        // Set the struct members appropriately
        if(port_values & (1<<0))
            sensor_values.outer_left = LINE;
        else
            sensor_values.outer_left = NO_LINE;

        if(port_values & (1<<1))
            sensor_values.line_left = LINE;
        else
            sensor_values.line_left = NO_LINE;

        if(port_values & (1<<2))
            sensor_values.line_right = LINE;
        else
            sensor_values.line_right = NO_LINE;

        if(port_values & (1<<3))
            sensor_values.outer_right = LINE;
        else
            sensor_values.outer_right = NO_LINE;

        return sensor_values;

    }

    /**
     * Read the status register, discarding its value.
     */
    void HardwareAbstractionLayer::clear_status_register() const
    {
        TRACE("clear_status_register()");
        DEBUG("Reading and discarding status register");
        this->rlink->request(STATUS);
    }

    /**
     * Read the status register and return it.
     * \returns The STATUS register
     */
    char HardwareAbstractionLayer::status_register() const
    {
        TRACE("status_register()");
        DEBUG("Reading status register");
        return static_cast<char>(this->rlink->request(STATUS));
    }

    /**
     * Read the reset switch and return its status.
     * \returns The current value of the switch, true if pressed
     */
    bool HardwareAbstractionLayer::reset_switch() const
    {
        TRACE("reset_switch()");
        return false;
    }

    /**
     * Read the switch mounted on the grabber arm and return its status.
     * \returns The current value of the switch, true if pressed
     */
    bool HardwareAbstractionLayer::grabber_switch() const
    {
        TRACE("grabber_switch()");
        return false;
    }

    /**
     * Get the analogue reading from the LDR used to detect colour.
     * \returns The analogue reading value
     */
    unsigned short int HardwareAbstractionLayer::colour_ldr() const
    {
        TRACE("colour_ldr()");
        return this->rlink->request(ADC0);
    }

    /**
     * Get the analogue reading from the LDR used to detect the bad bobbin.
     * \returns The analogue reading value
     */
    unsigned short int HardwareAbstractionLayer::bad_bobbin_ldr() const
    {
        TRACE("bad_bobbin_ldr()");
        return this->rlink->request(ADC1);
    }

    /**
     * Set the bobbin colour indication LEDs.
     * \param led_0 Whether LED0 should be on or off (true=on)
     * \param led_1 Whether LED1 should be on or off (true=on)
     * \param led_2 Whether LED2 should be on or off (true=on)
     */
    void HardwareAbstractionLayer::indication_LEDs(const bool led_0,
        const bool led_1, const bool led_2)
    {
        TRACE("indication_LEDs("<<led_0<<", "<<led_1<<", "<<led_2<<")");
        if (led_0)
            this->_port7 = ~(1<<1) & this->_port7;
        else
            this->_port7 = 1<<1 | this->_port7;
        
        if (led_1)
            this->_port7 = ~(1<<3) & this->_port7;
        else
            this->_port7 = 1<<3 | this->_port7;
        
        if (led_2)
            this->_port7 = ~(1<<5) & this->_port7;
        else
            this->_port7 = 1<<5 | this->_port7;
        
        this->rlink->command(WRITE_PORT_7, this->_port7);
    }

    /**
     * Turn on and off the LEDs used to light up the bobbin for colour
     * detection.
     * \param red Whether the red LED should be on or off (true=on)
     * \param green Whether the green LED should be on or off (true=on)
     */
    void HardwareAbstractionLayer::colour_LEDs(const bool red,
        const bool green)
    {
        TRACE("colour_leds("<<red<<", "<<green<<")");
        if (red)
            this->_port7 = ~(1<<0) & this->_port7;
        else
            this->_port7 = 1<<0 | this->_port7;

        if (green)
            this->_port7 = ~(1<<2) & this->_port7;
        else
            this->_port7 = ~(1<<2) & this->_port7;

        this->rlink->command(WRITE_PORT_7, this->_port7);
    }

    /**
     * Turn on and off the LED used to light up the top of the bobbin, for
     * bad bobbin detection.
     * \param status Whether the LED should be on or off (true=on)
     */
    void HardwareAbstractionLayer::bad_bobbin_LED(const bool status)
    {
        TRACE("bad_bobbin_led("<<status<<")");
        if (status)
            this->_port7 = ~(1<<4) & this->_port7; 
        else
            this->_port7 = 1<<4 | this->_port7; 

        this->rlink->command(WRITE_PORT_7, this->_port7);
    }

    /**
     * Turn the grabber jaw actuator on or off.
     * \param status Jaw actuator status (true=on)
     */
    void HardwareAbstractionLayer::grabber_jaw(const bool status)
    {
        TRACE("grabber_jaw("<<status<<")");
        if (status) {
            DEBUG("Clamping");
            this->_port7 = 1<<7 | this->_port7;
        } else {
            DEBUG("Releasing clamp");
            this->_port7 = ~(1<<7) & this->_port7;
        }

        this->rlink->command(WRITE_PORT_7, this->_port7);
    }

    /**
     * Turn the grabber lift mechanism actuator on or off.
     * \param status Lift actuator status (true=on)
     */
    void HardwareAbstractionLayer::grabber_lift(const bool status)
    {
        TRACE("grabber_lift("<<status<<")");
        if (status) {
            DEBUG("Lifting grabber");
            this->_port7 = 1<<6 | this->_port7;
        } else {
            DEBUG("Lowering grabber");
            this->_port7 = ~(1<<6) & this->_port7;
        }

        this->rlink->command(WRITE_PORT_7, this->_port7);
    }

    /**
     * Check the motor speed against the set maximum, printing an error
     * and returning true if the speed exceeds that value.
     * \param speed The speed to check
     * \return true if the speed is too high, false if okay
     */
    bool HardwareAbstractionLayer::check_max_speed(
        const unsigned short int speed) const
    {
        TRACE("check_max_speed("<<speed<<")");
        if(speed > MOTOR_MAX_SPEED) {
            DEBUG("ERROR: Motor speed too high, not setting");
            return true;
        } else {
            return false;
        }
    }
}

