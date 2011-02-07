// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.cc
// Navigation class implementation

#include "navigation.h"
#include "line_following.h"
#include "hal.h"

#include <iostream>

#define NAVIGATION_DEBUG 1

#if NAVIGATION_DEBUG
#define DEBUG(x) std::cout<<"[Navigation] "<<x<<std::endl
#else
#define DEBUG(x)
#endif

namespace IDP {

    /**
     * Initialise the class, storing the const pointer to the HAL.
     * \param hal A const pointer to an instance of the HAL
     */
    Navigation::Navigation(const HardwareAbstractionLayer* hal,
        const NavigationNode from=NODE7, const NavigationNode to=NODE8):
        _hal(hal), _from(from), _to(to), _lf(0), _cached_junction(NO_CACHE),
        _doing_second_turn(false)
    {
        DEBUG("Initialising Navigation");
        // Initialise a new lf object
        this->_lf = new LineFollowing(hal);
        this->_lf->set_speed(127);
    }

    /**
     * Destruct Navigation, deleting the LineFollowing object
     */
    Navigation::~Navigation()
    {
        if(this->_lf)
            delete this->_lf;
    }

    /**
     * Go to a NavigationLocation.
     * \returns A NavigationStatus code
     */
    const NavigationStatus Navigation::go(const NavigationLocation location)
    {
    }

    /**
     * Go to a particular NavigationNode
     * \returns A NavigationStatus code
     */
    const NavigationStatus Navigation::go_node(const NavigationNode target)
    {
        DEBUG("from: " << _from+1 << ", to: " << _to+1 <<
            ", target: " << target+1);
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

            if(this->_cached_junction == NO_CACHE) {
                LineFollowingStatus status = this->_lf->junction_status();
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
            if(this->_cached_junction == NO_TURNS) {
                // Still driving forwards
                // TODO catch this status and do something with it
                LineFollowingStatus status = this->_lf->follow_line();
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
}

