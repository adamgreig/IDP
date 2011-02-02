// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// hal.cc
// Hardware Abstraction Layer implementation

#include "hal.h"

#include <iostream>
#include <cstdlib>

#include <robot_instr.h>

namespace IDP {

    /**
     * Initialise the HAL class.
     * Establishes the link to the robot.
     */
    HardwareAbstractionLayer::HardwareAbstractionLayer(const int robot=0)
    {
        int status;
        this->rlink = new robot_link;

        // Initialise link
        std::cout << "[HAL] Initialising link" << std::endl;
        if(robot == 0) {
            status = this->rlink->initialise();
        } else {
            status = this->rlink->initialise(robot);
        }

        // Check link, exit on failure
        if(!status) {
            std::cerr << "[HAL] ERROR: Initialising rlink" << std::endl;
            std::cout << "[HAL] Could not initialise, quitting" << std::endl;
            std::exit(1);
            return;
        }

        // Set motor ramp speed
        this->rlink->command(RAMP_TIME, MOTOR_RAMP_TIME);
    }

    /**
     * Drive both motors forwards at a given speed.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_forward(
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Driving motors forward, speed " << speed;
        std::cout << std::endl;

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
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Driving motors backward, speed " << speed;
        std::cout << std::endl;

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_OPPOSITE, (0<<7) | speed);
    }

    /**
     * Drive the motors to steer the robot to the left.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_turn_left(
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Turning motors left, speed " << speed;
        std::cout << std::endl;

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_SAME, (1<<7) | speed);
    }

    /**
     * Drive the left motor forward at the given speed
     * \param speed The speed at which to drive the motor
     */
    void HardwareAbstractionLayer::motor_left_forward(
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Setting left motor forward at speed ";
        std::cout << speed << std::endl;

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
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Setting right motor forward at speed ";
        std::cout << speed << std::endl;

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
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Setting left motor backward at speed ";
        std::cout << speed << std::endl;

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
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Setting right motor backward at speed ";
        std::cout << speed << std::endl;

        if (this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(MOTOR_2_GO, (1<<7) speed);
    }

    /**
     * Drive the motors to steer the robot to the right.
     * \param speed The speed to drive at, 0 to 127
     */
    void HardwareAbstractionLayer::motors_turn_right(
        const unsigned short int speed) const
    {
        std::cout << "[HAL] Turning motors right, speed " << speed;
        std::cout << std::endl;

        if(this->check_max_speed(speed)) {
            return;
        }

        this->rlink->command(BOTH_MOTORS_GO_SAME, (0<<7) | speed);
    }

    /**
     * Stop all motors.
     */
    void HardwareAbstractionLayer::motors_stop() const
    {
        std::cout << "[HAL] Stopping all motors" << std::endl;
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
        //std::cout << "[HAL] Reading line following sensors" << std::endl;

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
        this->rlink->request(STATUS);
    }

    /**
     * Read the status register and return it.
     * \returns The STATUS register
     */
    const char HardwareAbstractionLayer::status_register() const
    {
        static_cast<const char>(return this->rlink->request(STATUS));
    }

    /**
     * Read the reset switch and return its status.
     * \returns The current value of the switch, true if pressed
     */
    bool HardwareAbstractionLayer::reset_switch() const
    {
    }

    /**
     * Read the switch mounted on the grabber arm and return its status.
     * \returns The current value of the switch, true if pressed
     */
    bool HardwareAbstractionLayer::grabber_switch() const
    {
    }

    /**
     * Get the analogue reading from the LDR used to detect colour.
     * \returns The analogue reading value
     */
    const unsigned short int HardwareAbstractionLayer::colour_ldr() const
    {
    }

    /**
     * Get the analogue reading from the LDR used to detect the bad bobbin.
     * \returns The analogue reading value
     */
    const unsigned short int HardwareAbstractionLayer::bad_bobbin_ldr() const
    {
    }

    /**
     * Set the bobbin colour indication LEDs.
     * \param led_0 Whether LED0 should be on or off (true=on)
     * \param led_1 Whether LED1 should be on or off (true=on)
     * \param led_2 Whether LED2 should be on or off (true=on)
     */
    void HardwareAbstractionLayer::indication_LEDs(const bool led_0,
        const bool led_1, const bool led_2) const
    {
    }

    /**
     * Turn on and off the LEDs used to light up the bobbin for colour
     * detection.
     * \param red Whether the red LED should be on or off (true=on)
     * \param green Whether the green LED should be on or off (true=on)
     */
    void HardwareAbstractionLayer::colour_leds(const bool red,
        const bool green) const
    {
    }

    /**
     * Turn on and off the LED used to light up the top of the bobbin, for
     * bad bobbin detection.
     * \param status Whether the LED should be on or off (true=on)
     */
    void HardwareAbstractionLayer::bad_bobbin_led(const bool status) const
    {
    }

    /**
     * Turn the grabber jaw actuator on or off.
     * \param status Jaw actuator status (true=on)
     */
    void HardwareAbstractionLayer::grabber_jaw(const bool status) const
    {
    }

    /**
     * Turn the grabber lift mechanism actuator on or off.
     * \param status Lift actuator status (true=on)
     */
    void HardwareAbstractionLayer::grabber_lift(const bool status) const
    {
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
        if(speed > MOTOR_MAX_SPEED) {
            std::cerr << "[HAL] ERROR: Motor speed too high, not setting";
            std::cerr << std::endl;
            return true;
        } else {
            return false;
        }
    }
}

