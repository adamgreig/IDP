// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.cc
// Navigation class implementation

#include "navigation.h"
#include "line_following.h"
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
     * Initialise the class, storing the const pointer to the HAL.
     *
     * The optional parameters from and to can be used to define the
     * starting position, but default to the 'start box'.
     *
     * \param hal A const pointer to an instance of the HAL
     * \param from The node behind the robot at the start
     * \param to The node in front of the robot at the start
     */
    Navigation::Navigation(HardwareAbstractionLayer* hal,
        const NavigationNode from=NODE7, const NavigationNode to=NODE8):
        _hal(hal), _from(from), _to(to), _lf(0), _cached_junction(NO_CACHE),
        _doing_second_turn(false)
    {
        TRACE("Navigation(" << hal << ", " << NavigationNodeStrings[from] <<
            ", " << NavigationNodeStrings[to] << ")");
        INFO("Initialising Navigation");
        
        // Initialise a new lf object
        this->_lf = new LineFollowing(hal);
        this->_lf->set_speed(127);
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
    }

    /**
     * Go to a NavigationLocation.
     * \returns A NavigationStatus code
     */
    NavigationStatus Navigation::go(const NavigationLocation location)
    {
        TRACE("go(" << NavigationLocationStrings[location] << ")");
        UNUSED(location);
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

        // Check if we need to turn, and do so
        if(this->turn_around_required(target))
            return this->turn_around();

        // Update our cached view of the junction status.
        // This stores the last known junction when we start
        // a turn so we don't get confused halfway through.
        this->update_cache();

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

        // Request LineFollowing to execute the required turn.
        LineFollowingStatus turnstatus;
        if(current_direction == NAVIGATION_CLOCKWISE) {
            DEBUG("Turning clockwise");
            turnstatus = this->_lf->turn_around_cw();
        } else {
            DEBUG("Turning anticlockwise");
            turnstatus = this->_lf->turn_around_ccw();
        }
        
        if(turnstatus == ACTION_COMPLETED) {
            // Swap around to and from nodes
            NavigationNode placeholder = this->_to;
            this->_to = this->_from;
            this->_from = placeholder;
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

        // See if we're there!
        if(target == this->_to) {
            DEBUG("Found target junction");
            return NAVIGATION_ARRIVED;
        }

        // Determine the current direction of travel around the course
        NavigationDirection current_direction;
        if(this->_to > this->_from) {
            DEBUG("We are travelling clockwise");
            current_direction = NAVIGATION_CLOCKWISE;
        } else {
            DEBUG("We are travelling anticlockwise");
            current_direction = NAVIGATION_ANTICLOCKWISE;
        }

        // Look up which direction to go
        NavigationTurn turn;
        turn = NAVIGATION_TURN_MAP[current_direction][this->_to];

        // Store the result of the LineFollowing operation
        LineFollowingStatus status;

        // Initialise status to a somewhat harmless default
        status = ACTION_IN_PROGRESS;

        if(turn == STRAIGHT) {
            DEBUG("Continuing straight over junction");
            status = this->_lf->follow_line();
        } else if(turn == LEFT) {
            DEBUG("Turning left");
            status = this->_lf->turn_left();
        } else if(turn == RIGHT) {
            DEBUG("Turning right");
            status = this->_lf->turn_right();
        } else if(turn == END_OF_LINE) {
            DEBUG("end of line :(");
            this->_hal->motors_stop();
            return NAVIGATION_ARRIVED;
        }

        // Special case the right hand turn after NODE6 as the
        // robot will see the starting box line and believe
        // that to be the end of the turn when in fact it is not.
        if(turn == RIGHT && this->_to == NODE6 &&
            current_direction == NAVIGATION_CLOCKWISE &&
            status == ACTION_COMPLETED && !this->_doing_second_turn)
        {
            INFO("Executing special case second right turn at NODE6");
            this->_doing_second_turn = true;
            return NAVIGATION_ENROUTE;
        }

        // If we're going straight, ACTION_IN_PROGRESS is an
        // indication of completion, while ACTION_COMPLETED means
        // a turn completed.
        if((turn == STRAIGHT && status == ACTION_IN_PROGRESS)
            || status == ACTION_COMPLETED)
        {
            DEBUG("Completed junction action");
            this->_doing_second_turn = false;
            this->_cached_junction = NO_CACHE;
            this->_from = this->_to;
            this->_to = NAVIGATION_ROUTE_MAP[current_direction][this->_to];
        }

        return NAVIGATION_ENROUTE;
    }
}

