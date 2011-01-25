// IDP
// Copyright 2011 Adam Greig & Jon Sowman
//
// main.cc
// Program entry point

#include <iostream>
#include <libidp/libidp.h>

int main()
{
    std::cout << "Hello, IDP..." << std::endl;
    IDP::MissionSupervisor* m;
    m = new IDP::MissionSupervisor();
    return 0;
}
