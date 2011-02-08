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
     * Find a route from one place to another on the board, and
     * maintain an estimate of the current position
     */
    class Navigation
    {
        public:
            Navigation(const HardwareAbstractionLayer* hal,
                const NavigationNode from, const NavigationNode to);
            ~Navigation();
            const NavigationStatus go(const NavigationLocation location);
            const NavigationStatus go_node(const NavigationNode target);
        private:
            const HardwareAbstractionLayer* _hal;
            LineFollowing* _lf;
            NavigationNode _from;
            NavigationNode _to;
            NavigationCachedJunction _cached_junction;
            bool _doing_second_turn;
    };
}

#endif /* LIBIDP_NAVIGATION_H */

