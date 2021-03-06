// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.cc
// Navigation class implementation

#include "navigation.h"
#include "line_following.h"
#include "clamp_control.h"
#include "hal.h"

// Debug functionality
#define MODULE_NAME "Navigation"
#define TRACE_ENABLED   true
#define DEBUG_ENABLED   true
#define INFO_ENABLED    true
#define ERROR_ENABLED   true
#include "debug.h"

// For now, define this to avoid unused-variable errors
#define UNUSED(x) (void)(x)

namespace IDP {

    /**
     * The turns at each node.
     *
     * Indexed by NavigationDirection and then by NavigationNode
     */
    const NavigationTurn NAVIGATION_NODE_TURNS[MAX_DIRECTION][MAX_NODE] = {
        {STRAIGHT, BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, BOTH,
            BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT,
            BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, RIGHT, END_OF_LINE},
        {END_OF_LINE, BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, RIGHT_AND_STRAIGHT,
            BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT,
            BOTH_AND_STRAIGHT, BOTH_AND_STRAIGHT, LEFT, STRAIGHT}
    };

    /**
     * Turns that should be taken at each node in each direction.
     *
     * Indexed by NavigationDirection and then by NavigationNode
     */
    const NavigationTurn NAVIGATION_TURN_MAP[MAX_DIRECTION][MAX_NODE] = {
        {STRAIGHT, STRAIGHT, STRAIGHT, RIGHT, STRAIGHT, RIGHT, STRAIGHT,
            STRAIGHT, STRAIGHT, RIGHT, END_OF_LINE},
        {END_OF_LINE, STRAIGHT, STRAIGHT, LEFT, STRAIGHT, LEFT, STRAIGHT,
            STRAIGHT, STRAIGHT, LEFT, STRAIGHT}
    };

    /**
     * The lookup table of NavigationLocations to a pair of NavigationNodes
     * indicating the start and end node (with implied direction).
     */
    const NavigationNode NAVIGATION_LOCATION_LOOKUP[MAX_LOCATION][2] = {
        {NODE9, NODE6}, // NAVIGATION_BOXES
        {NODE7, NODE10},// NAVIGATION_RACK
        {NODE3, NODE2}  // NAVIGATION_DELIVERY
    };

    /**
     * The route to take, node by node
     */
    const NavigationNode NAVIGATION_ROUTE_MAP[MAX_DIRECTION][MAX_NODE] = {
        {NODE2, NODE3, NODE4, NODE5, NODE6, NODE8, NODE8, NODE9, NODE10,
            NODE11, NODE10},
        {NODE2, NODE1, NODE2, NODE3, NODE4, NODE5, NODE6, NODE7, NODE8,
            NODE9, NODE10}
    };


    /**
     * Initialise the class, storing the pointer to the HAL.
     *
     * The optional parameters from and to can be used to define the
     * starting position, but default to the 'start box'.
     *
     * \param hal A const pointer to an instance of the HAL
     * \param from The node behind the robot at the start
     * \param to The node in front of the robot at the start
     */
    Navigation::Navigation(HardwareAbstractionLayer* hal,
        const NavigationNode from, const NavigationNode to):
        _hal(hal), _from(from), _to(to), _lf(0), _cc(0), 
        _cached_junction(NO_CACHE), _reached_special_case_junction(false)
    {
        TRACE("Navigation(" << hal << ", " << NavigationNodeStrings[from] <<
            ", " << NavigationNodeStrings[to] << ")");
        INFO("Initialising Navigation");
        
        // Initialise a new lf object
        this->_lf = new LineFollowing(hal);
        this->_lf->set_speed(127);

        // Initialise a new cc object
        this->_cc = new ClampControl(hal);
        this->_cc->open_jaw();
        this->_cc->lower_arm();
    }

    /**
     * Destruct Navigation, deleting the LineFollowing object
     */
    Navigation::~Navigation()
    {
        TRACE("~Navigation()");
        INFO("Destructing Navigation");
        if(this->_lf)
            delete this->_lf;
        if(this->_cc)
            delete this->_cc;
    }

    /**
     * Go and find a box and position the robot such that dropping a
     * bobbin will land it in the box.
     * \param box The Box to which to navigate for bobbin drop.
     * \returns A NavigationStatus code.
     */
    NavigationStatus Navigation::find_box_for_drop(Box box)
    {
        TRACE("find_box_for_drop(" << BoxStrings[box] << ")");

        // Reduce the speed whilst looking for a box so we don't
        // overshoot the node
        DEBUG("Setting speed to 80 to avoid node overshoot");
        this->_lf->set_speed(80);

        NavigationStatus nav_status;
        if (box == BOX1)
        {
            do {
                nav_status = this->go_node(NODE7);
            } while (nav_status == NAVIGATION_ENROUTE);
        }
        else if (box == BOX2)
        {
            do {
                nav_status = this->go_node(NODE8);
            } while (nav_status == NAVIGATION_ENROUTE);
        }

        DEBUG("Found a box!");
        this->_hal->motors_stop();
        return NAVIGATION_ARRIVED;
    }

    /**
     * Go and find a box and position the robot ready to pick the box
     * up, or analyse its contents.
     * \param box The Box to which to navigate for pickup.
     * \returns A NavigationStatus code.
     */
    NavigationStatus Navigation::find_box_for_pickup(Box box)
    {
        TRACE("find_box_for_pickup(" << BoxStrings[box] << ")");

        // Go to the relevant node for the box
        this->find_box_for_drop(box);

        // Slow down to find the box by reflection
        DEBUG("Reducing the speed to 48 for box detection");
        this->_lf->set_speed(48);

        // Stop while the jars and arms open
        DEBUG("Stopping the motors to wait for actuators");
        this->_hal->motors_stop();

        // Open the jaw and lower the arm
        DEBUG("Opening jaw and lowering arm");
        this->_cc->open_jaw();
        this->_cc->lower_arm();

        // Move slowly forwards until we detect a box top with the
        // badness LDR
        bool box_present;
        do {
            box_present = this->_cc->box_present();
            this->_lf->follow_line();
        } while (!box_present);

        // Set the speed back to normal ready to continue driving
        DEBUG("Found box!");
        DEBUG("Stopping motors");
        this->_hal->motors_stop();
        DEBUG("Resetting speed to 127");
        this->_lf->set_speed(127);

        return NAVIGATION_ARRIVED;
    }

    /**
     * Navigate to the starting box and commence a run along
     * the bobbin rack.
     * \returns A NavigationStatus code.
     */
    NavigationStatus Navigation::find_bobbin()
    {
        TRACE("find_bobbin()");

        // Get to the start box
        DEBUG("Moving to the start box");
        NavigationStatus nav_status;
        do {
            nav_status = this->go_node(NODE8);
        } while (nav_status == NAVIGATION_ENROUTE);

        // Stop while the jars and arms open
        DEBUG("Stopping the motors to wait for actuators");
        this->_hal->motors_stop();

        // Open the jaw and lower the arm
        DEBUG("Opening jaw and lowering arm");
        this->_cc->open_jaw();
        this->_cc->lower_arm();

        DEBUG("Ready to begin the bobbin run");

        // Start the bobbin run
        do {
            nav_status = this->find_next_bobbin();
        } while (nav_status == NAVIGATION_ENROUTE);

        DEBUG("Found a bobbin!");

        return nav_status;
    }

    /**
     * Drive forwards at a slow speed, and return only when a bobbin
     * is present.
     * \returns A NavigationStatus code
     */
    NavigationStatus Navigation::find_next_bobbin()
    {
        TRACE("find_next_bobbin()");
        
        // Reduce the speed of the robot
        DEBUG("Reducing speed to 20 for bobbin detection");
        this->_lf->set_speed(20);

        LineFollowingStatus lf_status;

        // Lose the current bobbin
        bool presence;
        DEBUG("Losing current bobbin");
        do {
            presence = this->_cc->bobbin_present();
            lf_status = this->_lf->follow_line();
        } while(presence);
        DEBUG("Lost current bobbin");

        if(lf_status == BOTH_TURNS_FOUND) {
            this->_from = NODE9;
            this->_to = NODE10;
        }

        // Follow the line until ClampControl says we're at a bobbin
        presence = this->_cc->bobbin_present();
        if (!presence) {
            lf_status = this->_lf->follow_line();
            return NAVIGATION_ENROUTE;
        }

        if(lf_status == BOTH_TURNS_FOUND) {
            this->_from = NODE9;
            this->_to = NODE10;
        }

        // Reset the speed back to full
        DEBUG("Got a bobbin, stopping & resetting speed to 127");
        this->_hal->motors_stop();
        this->_lf->set_speed(127);

        return NAVIGATION_ARRIVED;
    }

    /**
     * Navigate to the delivery area and align for delivery.
     * \returns A NavigationStatus code
     */
    NavigationStatus Navigation::go_to_delivery()
    {
        TRACE("go_to_delivery()");

        // Reduce speed to minimise positioning errors caused by inertia
        DEBUG("Reducing speed for delivery action");
        this->_lf->set_speed(80);

        NavigationStatus nav_status;
        do {
            nav_status = this->go_node(NODE3);
        } while(nav_status == NAVIGATION_ENROUTE);

        DEBUG("At node 3, turning...");

        LineFollowingStatus lf_status;
        do {
            lf_status = this->_lf->turn_around_delivery();
        } while(lf_status == ACTION_IN_PROGRESS);

        DEBUG("Finished delivery turn, ready to drop");

        // Reset the speed to full
        DEBUG("Resetting speed to 127");
        this->_lf->set_speed(127);

        return NAVIGATION_ARRIVED;
    }

    /**
     * Navigate out of the delivery area and get back onto the path.
     * \returns A NavigationStatus code
     */
    NavigationStatus Navigation::finished_delivery()
    {
        TRACE("finished_delivery()");

        // Reduce speed as this action can easily overshoot
        DEBUG("Setting speed to 80 to get back onto line");
        this->_lf->set_speed(80);

        DEBUG("Turning back onto the line towards node 4");
        LineFollowingStatus lf_status;
        do {
            lf_status = this->_lf->turn_around_cw();
        } while(lf_status == ACTION_IN_PROGRESS);

        DEBUG("Back on the line");

        this->_to = NODE4;
        this->_from = NODE3;

        this->_cached_junction = NO_CACHE;

        // Set the speed back to full
        DEBUG("Setting speed back to 127");
        this->_lf->set_speed(127);

        return NAVIGATION_ARRIVED;
    }

    /**
     * Go to a particular NavigationNode
     * \returns A NavigationStatus code
     */
    NavigationStatus Navigation::go_node(const NavigationNode target)
    {
        TRACE("go_node(" << NavigationNodeStrings[target] << ")");
        DEBUG("From " << NavigationNodeStrings[_from] << ", to " <<
            NavigationNodeStrings[_to] << ", target " <<
            NavigationNodeStrings[target]);

        // Update our cached view of the junction status.
        // This stores the last known junction when we start
        // a turn so we don't get confused halfway through.
        this->update_cache();

        // Check if we need to turn, and do so
        if(this->turn_around_required(target))
            return this->turn_around();

        // If we detect a junction, handle it, otherwise keep on
        // driving straight.
        if(this->_cached_junction == NO_TURNS) {
            this->_cached_junction = NO_CACHE;
            LineFollowingStatus status = this->_lf->follow_line();
            if(status == LOST)
                return NAVIGATION_LOST;
            return NAVIGATION_ENROUTE;
        } else {
            return this->handle_junction(target);
        }

    }

    /**
     * Return to home, which is to say the starting box. In addition,
     * ensure we are facing the right direction (clockwise)
     */
    NavigationStatus Navigation::go_home()
    {
        TRACE("go_home()");
        NavigationStatus status = this->go_node(NODE8);
        return status;
    }

    /**
     * Check if we need to turn around
     * \param target The NavigationNode we need to go to
     * \returns A bool, true if we need to turn around
     */
    bool Navigation::turn_around_required(const NavigationNode target) const
    {
        TRACE("turn_around_required(" << NavigationNodeStrings[target]<<")");

        // If target is to, we don't need to turn.
        if(target == this->_to)
            return false;

        // Determine direction we need to be travelling to get to
        // the target, and also the direction we are currently
        // travelling.
        NavigationDirection target_direction, current_direction;
        if(target > this->_to)
            target_direction = NAVIGATION_CLOCKWISE;
        else
            target_direction = NAVIGATION_ANTICLOCKWISE;
        if(this->_to > this->_from)
            current_direction = NAVIGATION_CLOCKWISE;
        else
            current_direction = NAVIGATION_ANTICLOCKWISE;

        return target_direction != current_direction;
    }

    /**
     * Determine the current direction of travel and then execute an
     * about turn.
     * \returns A NavigationStatus of NAVIGATION_ENROUTE if currently
     * turning, or NAVIGATION_LOST if line following got lost during
     * the turn.
     */
    NavigationStatus Navigation::turn_around()
    {
        TRACE("turn_around()");

        // Determine the current direction of travel around the course
        NavigationDirection current_direction;
        if(this->_to > this->_from) {
            DEBUG("We are travelling clockwise");
            current_direction = NAVIGATION_CLOCKWISE;
        } else {
            DEBUG("We are travelling anticlockwise");
            current_direction = NAVIGATION_ANTICLOCKWISE;
        }

        // If we're in the start box and facing ccw, get to node 6 and then
        // turn around. this is a bit awful in that it uses a loop, but times
        // needs must
        if((this->_from == NODE8 && this->_to == NODE7) ||
           (this->_from == NODE7 && this->_to == NODE6))
        {
            //this->_lf->set_speed(30);
            NavigationStatus status;
            do {
                status = this->go_node(NODE6);
            } while(status == NAVIGATION_ENROUTE);

            LineFollowingStatus lf_status;
            do {
                lf_status = this->_lf->turn_around_cw(2);
            } while(lf_status == ACTION_IN_PROGRESS);

            this->_from = NODE7;
            this->_to = NODE8;
            this->_cached_junction = NO_CACHE;
            this->_reached_special_case_junction = false;
            return NAVIGATION_ENROUTE;
        }

        // If we're starting in the start box, leave before turning
        if(this->_from == NODE7 && this->_to == NODE8) {
            DEBUG("Leaving the start box before turning");
            LineFollowingStatus forwardstatus;
            forwardstatus = this->_lf->follow_line();
            if(forwardstatus == ACTION_IN_PROGRESS) {
                DEBUG("LF is in progress *");
                return NAVIGATION_ENROUTE;
            } else if(forwardstatus == LOST) {
                return NAVIGATION_LOST;
            } else {
                this->_cached_junction = BOTH_TURNS;
                this->handle_junction(NODE9);
                return NAVIGATION_ENROUTE;
            }
        }

        // Turning around is a dangerous business!
        // Mostly we only want to turn around between nodes 8 and 9, or 9 and
        // 10, as we drive along picking up bobbins. In either of these cases,
        // drive until we get to the end node, then do the turn safely as we
        // know where we are. In other cases, just turn and hope for the best -
        // this is pretty unlikely to happen.
        if(((this->_from == NODE8 && this->_to == NODE9) ||
           (this->_from == NODE9 && this->_to == NODE10)) &&
            !this->_reached_special_case_junction)
        {
            DEBUG("Turning around on top line, special casing");
            LineFollowingStatus forwardstatus;
            forwardstatus = this->_lf->follow_line();
            if(forwardstatus == ACTION_IN_PROGRESS) {
                DEBUG("LF is in progress");
                this->_cached_junction = NO_CACHE;
                return NAVIGATION_ENROUTE;
            } else if(forwardstatus == LOST) {
                DEBUG("Got lost");
                return NAVIGATION_LOST;
            } else {
                // We found the junction, so tell LF to skip a line
                // and then do the turn.
                DEBUG("Found end junction on turn around special case");
                this->_reached_special_case_junction = true;
                this->_cached_junction = NO_CACHE;
            }
        }

        unsigned short int skip_lines = 0;
        if(this->_reached_special_case_junction)
            //skip_lines = 1;
            skip_lines = 0;

        DEBUG("skip_lines="<<skip_lines<<", reached_s_c_j="<<
                this->_reached_special_case_junction);

        // Request LineFollowing to execute the required turn.
        LineFollowingStatus turnstatus;
        if(current_direction == NAVIGATION_CLOCKWISE) {
            DEBUG("Turning clockwise");
            turnstatus = this->_lf->turn_around_cw(skip_lines);
        } else {
            DEBUG("Turning anticlockwise");
            turnstatus = this->_lf->turn_around_ccw(skip_lines);
        }
        
        if(turnstatus == ACTION_COMPLETED) {
            // Swap around to and from nodes
            NavigationNode placeholder = this->_to;
            this->_to = this->_from;
            this->_from = placeholder;
            this->_reached_special_case_junction = false;
            this->_cached_junction = NO_CACHE;
        } else if(turnstatus == LOST) {
            // If lost, bubble that up
            return NAVIGATION_LOST;
        }

        return NAVIGATION_ENROUTE;
    }

    /**
     * Check if we should update the cached junction status and
     * request a new one from LineFollowing if required.
     */
    void Navigation::update_cache()
    {
        TRACE("update_cache()");

        if(this->_cached_junction == NO_CACHE) {
            LineFollowingStatus status = this->_lf->junction_status();
            DEBUG("Updating cache with " << LineFollowingStatusStrings[status]);
            if(status == NO_TURNS_FOUND)
                this->_cached_junction = NO_TURNS;
            else if(status == LEFT_TURN_FOUND)
                this->_cached_junction = LEFT_TURN;
            else if(status == RIGHT_TURN_FOUND)
                this->_cached_junction = RIGHT_TURN;
            else if(status == BOTH_TURNS_FOUND)
                this->_cached_junction = BOTH_TURNS;
        } else {
            DEBUG("Using cached status " << this->_cached_junction);
        }
    }

    /**
     * Handle arriving at a junction, either the target junction or
     * an intermediate one where a turn may be required.
     *
     * If we are at the target junction, return with NAVIGATION_ARRIVED,
     * or at an intermediate junction look up the required action and
     * then tell LineFollowing whether to go straight forward or turn,
     * and keep calling Navigation::turn() as appropriate to execute
     * the turn.
     *
     * \returns NAVIGATION_ARRIVED if at the target junction, or
     * NAVIGATION_ENROUTE otherwise.
     */
    NavigationStatus Navigation::handle_junction(const NavigationNode target)
    {
        TRACE("handle_junction("<<NavigationNodeStrings[target]<<")");

        // Determine the current direction of travel around the course
        NavigationDirection current_direction;
        if(this->_to > this->_from) {
            DEBUG("We are travelling clockwise");
            current_direction = NAVIGATION_CLOCKWISE;
        } else {
            DEBUG("We are travelling anticlockwise");
            current_direction = NAVIGATION_ANTICLOCKWISE;
        }

        // See if we're there!
        if(target == this->_to) {
            DEBUG("Found target junction");
            this->_from = this->_to;
            this->_to = NAVIGATION_ROUTE_MAP[current_direction][this->_to];
            return NAVIGATION_ARRIVED;
        }

        // Look up which direction to go
        NavigationTurn turn;
        turn = NAVIGATION_TURN_MAP[current_direction][this->_to];

        // Store the result of the LineFollowing operation
        LineFollowingStatus status;

        // Initialise status to a somewhat harmless default
        status = ACTION_IN_PROGRESS;

        // Number of lines to skip on the turn.
        // Almost always zero, except when we are doing a
        // right hand turn after NODE6 due to the starting
        // box getting in the way.
        unsigned short int skip_lines = 0;
        if(turn == RIGHT && this->_to == NODE6 &&
           current_direction == NAVIGATION_CLOCKWISE)
        {
            DEBUG("Telling LineFollowing to skip one line for NODE6");
            skip_lines = 1;
        }

        if(turn == STRAIGHT) {
            DEBUG("Continuing straight over junction");
            status = this->_lf->follow_line();
        } else if(turn == LEFT) {
            DEBUG("Turning left");
            status = this->_lf->turn_left();
        } else if(turn == RIGHT) {
            DEBUG("Turning right");
            status = this->_lf->turn_right(skip_lines);
        } else if(turn == END_OF_LINE) {
            DEBUG("end of line :(");
            this->_hal->motors_stop();
            return NAVIGATION_ARRIVED;
        }

        // If we're going straight, ACTION_IN_PROGRESS is an
        // indication of completion, while ACTION_COMPLETED means
        // a turn completed.
        if((turn == STRAIGHT && status == ACTION_IN_PROGRESS)
            || status == ACTION_COMPLETED)
        {
            DEBUG("Completed junction action");
            this->_cached_junction = NO_CACHE;
            this->_from = this->_to;
            this->_to = NAVIGATION_ROUTE_MAP[current_direction][this->_to];
        }

        return NAVIGATION_ENROUTE;
    }
}

