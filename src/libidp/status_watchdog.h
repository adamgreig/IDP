// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// status_watchdog.h
// Status watchdog class definition
//
// Polls the STATUS register of the microcontroller and handles any
// errors that may arise

#pragma once
#ifndef LIBIDP_H
#define LIBIDP_H

namespace IDP {

    /**
     * Polls the STATUS register of the microcontroller any handles
     * any errors that may arise
     */
    class StatusWatchdog {
        public:
            const int check() const;
    };
}

#endif /* LIBIDP_H */

