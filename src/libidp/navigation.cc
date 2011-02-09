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
#define TRACE_ENABLED   false
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
     * String representations of NavigationStatus
     */
    const char* NavigationStatusStrings[] = {
        "NAVIGATION_ENROUTE", "NAVIGATION_ARRIVED", "NAVIGATION_LOST",
        "MAX_STATUS"
    };

    /**
     * String representations of NavigationLocation
     */
    const char* NavigationLocationStrings[] = {
        "NAVIGATION_BOXES", "NAVIGATION_RACK", "NAVIGATION_DELIVERY",
        "MAX_LOCATION"
    };

    /**
     * String representations of NavigationDirection
     */
    const char* NavigationDirectionStrings[] = {
        "NAVIGATION_CLOCKWISE", "NAVIGATION_ANTICLOCKWISE", "MAX_DIRECTION"
    };

    /**
     * String representations of NavigationNode
     */
    const char* NavigationNodeStrings[] = {
        "NODE1", "NODE2", "NODE3", "NODE4", "NODE5", "NODE6", "NODE7",
        "NODE8", "NODE9", "NODE10", "NODE11", "MAX_NODE"
    };

    /**
     * String representations of NavigationTurn
     */
    const char* NavigationTurnStrings[] = {
        "STRIGHT", "LEFT", "RIGHT", "BOTH", "LEFT_AND_STRAIGHT",
        "RIGHT_AND_STRAIGHT", "BOTH_AND_STRAIGHT", "END_OF_LINE", "MAX_TURNS"
    };

    /**
     * String representations of NavigationCachedJunction
     */
    const char* NavigationCachedJunctionStrings[] = {
        "NO_CACHE", "LEFT_TURN", "RIGHT_TURN", "BOTH_TURNS", "NO_TURNS"
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

        if(target == this->_to) {
            // Already heading in the right direction
            LineFollowingStatus status = this->_lf->follow_line();
            if(status == ACTION_IN_PROGRESS) {
                // Still driving
                return NAVIGATION_ENROUTE;
            } else if(status == LOST) {
                DEBUG("LineFollowing got lost :(");
                return NAVIGATION_LOST;
            } else {
                // Found the junction
                // Later, check this is the correct junction
                DEBUG("Found target junction");
                return NAVIGATION_ARRIVED;
            }
        } else {
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

            // If we need to turn around, work out which way and
            // do the turn.
            if(target_direction != current_direction) {
                DEBUG("Need to turn around");
                LineFollowingStatus turnstatus;
                if(current_direction == NAVIGATION_CLOCKWISE)
                    turnstatus = this->_lf->turn_around_cw();
                else
                    turnstatus = this->_lf->turn_around_ccw();
                if(turnstatus == ACTION_IN_PROGRESS)
                    return NAVIGATION_ENROUTE;
                else if(turnstatus == LOST)
                    return NAVIGATION_LOST;

                DEBUG("Turn completed");

                // Swap to and from now that we've finished the turn
                NavigationNode placeholder = this->_to;
                this->_to = this->_from;
                this->_from = placeholder;
                current_direction = target_direction;
            }

            // If nothing is in the cache, update it, otherwise
            // leave it alone.
            this->update_cache();

            if(this->_cached_junction == NO_TURNS) {
                // Still driving forwards
                // TODO catch this status and do something with it
                LineFollowingStatus status = this->_lf->follow_line();
                UNUSED(status);
                this->_cached_junction = NO_CACHE; 
                return NAVIGATION_ENROUTE;
            } else {
                DEBUG("Arrived at junction " << this->_to + 1);
                // We are at a junction or EOL
                if(this->_to == target) {
                    DEBUG("Arrived at target! :)");
                    return NAVIGATION_ARRIVED;
                } else {
                    // Look up which direction to go
                    NavigationTurn turn;
                    turn = NAVIGATION_TURN_MAP[current_direction][this->_to];
                    if(turn == STRAIGHT) {
                        // Go straight. Check we're over the junction by
                        // ensuring we get ACTION_IN_PROGRESS back before
                        // updating our position.
                        DEBUG("Continuing straight over junction");
                        LineFollowingStatus status = this->_lf->follow_line();
                        if(status == ACTION_IN_PROGRESS) {
                            DEBUG("Driven over juntion");
                            this->_cached_junction = NO_CACHE; 
                            this->_from = this->_to;
                            this->_to = NAVIGATION_ROUTE_MAP
                                [current_direction][this->_to];
                        }
                    } else if(turn == LEFT) {
                        DEBUG("Turning left");
                        LineFollowingStatus status = this->_lf->turn_left();
                        if(status == ACTION_COMPLETED) {
                            DEBUG("Left turn completed");
                            this->_cached_junction = NO_CACHE; 
                            this->_from = this->_to;
                            this->_to = NAVIGATION_ROUTE_MAP
                                [current_direction][this->_to];
                        }
                    } else if(turn == RIGHT) {
                        DEBUG("Turning right");
                        LineFollowingStatus status = this->_lf->turn_right();
                        if(status == ACTION_COMPLETED) {
                            DEBUG("Right turn completed");
                            if(this->_to == NODE6 &&
                                    current_direction == NAVIGATION_CLOCKWISE){
                                if(!this->_doing_second_turn) {
                                    this->_doing_second_turn = true;
                                    return NAVIGATION_ENROUTE;
                                }
                            }
                            this->_doing_second_turn = false;
                            this->_cached_junction = NO_CACHE; 
                            this->_from = this->_to;
                            this->_to = NAVIGATION_ROUTE_MAP
                                [current_direction][this->_to];
                        }
                    } else if(turn == END_OF_LINE) {
                        DEBUG("end of line :(");
                        this->_hal->motors_stop();
                        return NAVIGATION_ARRIVED;
                    }
                    return NAVIGATION_ENROUTE;
                }
            }
        }
    }

    /**
     * Check if we need to turn around, and if so set that up.
     * \param target The NavigationNode we need to go to
     * \returns A NavigationTurnStatus of ACTION_IN_PROGRESS if
     * we are turning, or ACTION_COMPLETED if a turn is complete
     * or was not required.
     */
    NavigationTurnStatus Navigation::check_turn_around(
        const NavigationNode target)
    {
    }

    /**
     * Handle completing a turn left, right, or around clockwise or
     * anticlockwise.
     * \return A NavigationTurnStatus of ACTION_IN_PROGRESS if currently
     * turning, or ACTION_COMPLETED if the turn is complete.
     */
    NavigationTurnStatus Navigation::turn()
    {
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
    }
}

