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
    MissionSupervisor::MissionSupervisor(int robot = 0)
    {
        TRACE("MissionSupervisor(" << robot << ")");
        INFO("Constructing a MisionSupervisor, robot=" << robot);

        // Construct the hardware abstraction layer
        this->_hal = new HardwareAbstractionLayer(robot);
    }

    /**
     * Destruct the MissionSupervisor, deleting the HAL
     */
    MissionSupervisor::~MissionSupervisor()
    {
        TRACE("~MissionSupervisor()");
        delete this->_hal;
    }

    /**
     * Commence running the main task
     */
    void MissionSupervisor::run_task()
    {
        TRACE("run_task()");
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

