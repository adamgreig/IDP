// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// mission_supervisor.cc
// Mission Supervisor class implementation

#include <iostream>
#include <robot_instr.h>

#include "mission_supervisor.h"
#include "hal.h"
#include "line_following.h"
#include "navigation.h"
#include "clamp_control.h"

// Debug functionality
#define MODULE_NAME "MisSup"
#define TRACE_ENABLED   false
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

namespace IDP {
    /**
     * Construct the MissionSupervisor.
     * Initialises a link to the specified robot number, or 0 if running
     * embedded.
     * \param robot Which robot to link to, or 0 if embedded
     */
    MissionSupervisor::MissionSupervisor(int robot = 0):
        _hal(0), _nav(0), _cc(0), _box_has_red(false), _box_has_green(false),
        _box_has_white(false), _already_delivered_box_one(false)
    {
        TRACE("MissionSupervisor(" << robot << ")");
        INFO("Constructing a MisionSupervisor, robot=" << robot);

        // Construct the hardware abstraction layer
        this->_hal = new HardwareAbstractionLayer(robot);

        // Construct a Navigation
        this->_nav = new Navigation(this->_hal);

        // Construct a ClampControl
        this->_cc = new ClampControl(this->_hal);

        this->_cc->store_zero();
    }

    /**
     * Destruct the MissionSupervisor, deleting the HAL
     */
    MissionSupervisor::~MissionSupervisor()
    {
        TRACE("~MissionSupervisor()");
        if(this->_hal)
            delete this->_hal;
        if(this->_nav)
            delete this->_nav;
        if(this->_cc)
            delete this->_cc;
    }

    /**
     * Export the internal state via pass-by-reference variables.
     * \param red Whether a red bobbin is in the box.
     * \param green Whether a green bobbin is in the box.
     * \param white Whether a white bobbin is in the box.
     * \param box1 Whether the first box has already been delivered.
     */
    void MissionSupervisor::export_state(bool &red, bool &green, bool &white,
            bool &box1)
    {
        TRACE("export_state(..)");
        INFO("Exporting state from Mission Supervisor");
        red = this->_box_has_red;
        green = this->_box_has_green;
        white = this->_box_has_white;
        box1 = this->_already_delivered_box_one;
    }

    /**
     * Load state from provided values.
     * \param red Whether a red bobbin is in the box.
     * \param green Whether a green bobbin is in the box.
     * \param white Whether a white bobbin is in the box.
     * \param box1 Whether the first box has already been delivered.
     */
    void MissionSupervisor::load_state(bool red, bool green, bool white,
            bool box1)
    {
        TRACE("load_state("<<red<<", "<<green<<", "<<white<<", "<<box1);
        INFO("Importing state: " << red << green << white << box1);
        this->_box_has_red = red;
        this->_box_has_green = green;
        this->_box_has_white = white;
        this->_already_delivered_box_one = box1;
    }

    /**
     * Commence running the main task. Return when two filled boxes have
     * been delivered to the delivery area.
     */
    void MissionSupervisor::run_task()
    {
        TRACE("run_task()");

        INFO("Starting task run");

        if(!this->_already_delivered_box_one) {
            INFO("Filling box one");
            this->fill_and_deliver(BOX1);
            this->_already_delivered_box_one = true;
        }

        INFO("Filling box two");
        this->fill_and_deliver(BOX2);

        INFO("All done!");

    }

    /**
     * From the start zone, go to the given box, identify its colour,
     * then find the complementary two bobbins and place them in the box
     * before picking the box up, driving to the delivery area and dropping
     * it off, then return to the start.
     * \param box Which Box to fill
     */
    void MissionSupervisor::fill_and_deliver(Box box)
    {
        NavigationStatus nav_status;

        // We're at the start zone to begin with, so navigate to the box.
        INFO("Navigating to the box (" << BoxStrings[box] << ")");
        do {
            nav_status = this->_nav->find_box_for_pickup(box);
        } while(nav_status == NAVIGATION_ENROUTE);

        // Ensure the grabber jaw is open, then lower the arm to the box
        INFO("Lowering arm to box");
        this->_cc->open_jaw();
        this->_cc->lower_arm();

        // Check box colour
        INFO("Checking box colour...");
        BobbinColour box_colour = this->_cc->box_colour();
        INFO("Detected box colour: " << BobbinColourStrings[box_colour]);

        // Store box contents status
        this->_box_has_red = false;
        this->_box_has_white = false;
        this->_box_has_green = false;

        this->update_box_contents(box_colour);

        int bobbin_count;
        INFO("Going to find some bobbins!");
        for(bobbin_count = 0; bobbin_count < 2; bobbin_count++) {
            // Go find the first bobbin on the rack
            INFO("Going to the first bobbin on the rack");
            do {
                nav_status = this->_nav->find_bobbin();
            } while(nav_status == NAVIGATION_ENROUTE);

            // Check bobbin colour and move to next until we find something
            // we like
            BobbinColour bobbin_colour = this->find_useful_bobbin();

            // Pick the bobbin up
            INFO("Picking the bobbin up");
            this->_cc->raise_arm();

            // Return to our box
            INFO("Returning to box");
            do {
                nav_status = this->_nav->find_box_for_drop(box);
            } while(nav_status == NAVIGATION_ENROUTE);

            // Drop the bobbin
            INFO("Putting the bobbin down in the box");
            this->_cc->put_down();

            // Update box contents
            this->update_box_contents(bobbin_colour);

        }
        
        INFO("Box filled! Delivery time.");
        this->_nav->find_box_for_pickup(box);

        INFO("Picking box up");
        this->_cc->pick_up();

        INFO("Taking box to delivery");
        do {
            nav_status = this->_nav->go_to_delivery();
        } while(nav_status == NAVIGATION_ENROUTE);

        INFO("Delivering box");
        this->_cc->put_down();

        INFO("Leaving delivery zone");
        do {
            nav_status = this->_nav->finished_delivery();
        } while(nav_status == NAVIGATION_ENROUTE);

        INFO("Returning to start zone");
        do {
            nav_status = this->_nav->go_home();
        } while(nav_status == NAVIGATION_ENROUTE);

        INFO("Back home");
    }

    /**
     * Stop.
     */
    void MissionSupervisor::stop()
    {
        this->_hal->motors_stop();
    }

    /**
     * Update the current box contents given the new colour
     * \param colour Which colour to put into the box
     */
    void MissionSupervisor::update_box_contents(BobbinColour colour)
    {
        TRACE("update_box_contents(" << BobbinColourStrings[colour] << ")");
        INFO("Updating box contents to include " <<
            BobbinColourStrings[colour]);

        if(colour == BOBBIN_WHITE)
            this->_box_has_white = true;
        else if(colour == BOBBIN_RED)
            this->_box_has_red = true;
        else if(colour == BOBBIN_GREEN)
            this->_box_has_green = true;
    }

    /**
     * Go to the first bobbin, check its colour, then keep moving down
     * the rack until we find a bobbin we like.
     * TODO: termination conditions!
     */
    BobbinColour MissionSupervisor::find_useful_bobbin()
    {
        BobbinColour bobbin_colour;
        BobbinBadness badness;
        for(;;) {
            // Check the colour
            this->_cc->close_jaw();
            bobbin_colour = this->_cc->colour();
            badness = this->_cc->badness();
            if(((!this->_box_has_white && bobbin_colour == BOBBIN_WHITE) ||
               (!this->_box_has_red   && bobbin_colour == BOBBIN_RED  ) ||
               (!this->_box_has_green && bobbin_colour == BOBBIN_GREEN)) &&
                badness == BOBBIN_GOOD)
            {
                // Stop looking if it's a good colour
                INFO("Found a colour we like (" << 
                    BobbinColourStrings[bobbin_colour] << ")");
                break;
            } else {
                // Go to the next bobbin
                INFO("Don't like this bobbin, moving on (it was " <<
                    BobbinColourStrings[bobbin_colour] << ")");
                if (badness == BOBBIN_BAD)
                    INFO("This bobbin was bad");
                this->_cc->open_jaw();
                NavigationStatus nav_status;
                do {
                    nav_status = this->_nav->find_next_bobbin();
                } while(nav_status == NAVIGATION_ENROUTE);
            }
        }

        return bobbin_colour;
    }

    /**
     * Const accessor for the HAL
     */
    const HardwareAbstractionLayer* MissionSupervisor::hal() const
    {
        TRACE("hal()");
        return static_cast<const HardwareAbstractionLayer*> (this->_hal);
    }
}

