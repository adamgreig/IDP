// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// .h
// class definition
//

#pragma once
#ifndef LIBIDP_H
#define LIBIDP_H

#include <robot_link.h>

namespace IDP {
    class StatusWatchdog {
        public:
            const int check() const;
    }
}

#endif /* LIBIDP_H */

