// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// navigation.h
// Navigation class definition
//
// Navigation - Routefinding and position estimation control.

#pragma once
#ifndef LIBIDP_NAVIGATION_H
#define LIBIDP_NAVIGATION_H

namespace IDP {
    
    class HardwareAbstractionLayer;
    class LineFollowing;

    /**
     * Current navigation status
     */
    enum NavigationStatus {
        NAVIGATION_ENROUTE, NAVIGATION_ARRIVED, NAVIGATION_LOST, MAX_STATUS
    };

    /**
     * Possible locations for navigation to be asked to go to
     */
    enum NavigationLocation {
        NAVIGATION_BOXES, NAVIGATION_RACK, NAVIGATION_DELIVERY, MAX_LOCATION
    };

    /**
     * Directions around the circuit
     */
    enum NavigationDirection {
        NAVIGATION_CLOCKWISE, NAVIGATION_ANTICLOCKWISE, MAX_DIRECTION
    };

    /**
     * Navigation nodes, numbered clockwise from the bottom right corner
     * of the table.
     */
    enum NavigationNode {
        NODE1, NODE2, NODE3, NODE4, NODE5, NODE6, NODE7, NODE8, NODE9, NODE10,
        NODE11, MAX_NODE
    };

    /**
     * Possible turns at a node
     */
    enum NavigationTurn {
        STRAIGHT, LEFT, RIGHT, BOTH, LEFT_AND_STRAIGHT, RIGHT_AND_STRAIGHT,
        BOTH_AND_STRAIGHT, END_OF_LINE, MAX_TURNS
    };

    /**
     * Cached junction information for use while driving over a junction
     * or executing a turn.
     */
    enum NavigationCachedJunction {
        NO_CACHE, LEFT_TURN, RIGHT_TURN, BOTH_TURNS, NO_TURNS
    };

    /**
     * Find a route from one place to another on the board, and
     * maintain an estimate of the current position
     */
    class Navigation
    {
        public:
            Navigation(HardwareAbstractionLayer* hal,
                const NavigationNode from, const NavigationNode to);
            ~Navigation();
            NavigationStatus go(const NavigationLocation location);
            NavigationStatus go_node(const NavigationNode target);
        private:
            HardwareAbstractionLayer* _hal;
            NavigationNode _from;
            NavigationNode _to;
            LineFollowing* _lf;
            NavigationCachedJunction _cached_junction;
            bool _doing_second_turn;
    };
}

#endif /* LIBIDP_NAVIGATION_H */

