# IDP CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(librobot CXX)
cmake_minimum_required(VERSION 2.6)

if($ENV{ARM})
add_library(robot.arm STATIC IMPORTED)
set_property(TARGET robot.arm PROPERTY IMPORTED_LOCATION librobot.a)
else()
add_library(robot STATIC IMPORTED)
set_property(TARGET robot PROPERTY IMPORTED_LOCATION librobot.a)
endif()
