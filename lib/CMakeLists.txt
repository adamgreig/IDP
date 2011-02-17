# IDP L108 CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(IDPLibs CXX)
cmake_minimum_required(VERSION 2.6)

# Build google test and mock
#add_subdirectory("gmock")
#enable_testing()

# Add librobot library
add_subdirectory("librobot")
