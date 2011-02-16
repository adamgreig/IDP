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
     * Box identifiers
     */
    enum Box {
        BOX1, BOX2, MAX_BOX
    };

    /**
     * Cached junction information for use while driving over a junction
     * or executing a turn.
     */
    enum NavigationCachedJunction {
        NO_CACHE, LEFT_TURN, RIGHT_TURN, BOTH_TURNS, NO_TURNS
    };

    /**
     * String representations of NavigationStatus
     */
    static const char* const NavigationStatusStrings[] = {
        "NAVIGATION_ENROUTE", "NAVIGATION_ARRIVED", "NAVIGATION_LOST",
        "MAX_STATUS"
    };

    /**
     * String representations of NavigationLocation
     */
    static const char* const NavigationLocationStrings[] = {
        "NAVIGATION_BOXES", "NAVIGATION_RACK", "NAVIGATION_DELIVERY",
        "MAX_LOCATION"
    };

    /**
     * String representations of NavigationDirection
     */
    static const char* const NavigationDirectionStrings[] = {
        "NAVIGATION_CLOCKWISE", "NAVIGATION_ANTICLOCKWISE", "MAX_DIRECTION"
    };

    /**
     * String representations of NavigationNode
     */
    static const char* const NavigationNodeStrings[] = {
        "NODE1", "NODE2", "NODE3", "NODE4", "NODE5", "NODE6", "NODE7",
        "NODE8", "NODE9", "NODE10", "NODE11", "MAX_NODE"
    };

    /**
     * String representations of NavigationTurn
     */
    static const char* const NavigationTurnStrings[] = {
        "STRIGHT", "LEFT", "RIGHT", "BOTH", "LEFT_AND_STRAIGHT",
        "RIGHT_AND_STRAIGHT", "BOTH_AND_STRAIGHT", "END_OF_LINE", "MAX_TURNS"
    };

    /**
     * String representation of Box
     */
    static const char* const BoxStrings[] = {
        "BOX1", "BOX2"
    };

    /**
     * String representations of NavigationCachedJunction
     */
    static const char* const NavigationCachedJunctionStrings[] = {
        "NO_CACHE", "LEFT_TURN", "RIGHT_TURN", "BOTH_TURNS", "NO_TURNS"
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
            NavigationStatus find_bobbin();
            NavigationStatus find_box(Box box);
            NavigationStatus go_to_delivery();
            NavigationStatus finished_delivery();
            NavigationStatus go_node(const NavigationNode target);
            NavigationStatus go_home();
        private:
            void update_cache();
            bool turn_around_required(const NavigationNode target) const;
            NavigationStatus turn_around();
            NavigationStatus handle_junction(const NavigationNode target);
            HardwareAbstractionLayer* _hal;
            NavigationNode _from;
            NavigationNode _to;
            LineFollowing* _lf;
            NavigationCachedJunction _cached_junction;
    };
}

#endif /* LIBIDP_NAVIGATION_H */

