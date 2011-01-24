# IDP L108 CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(libidp CXX)
cmake_minimum_required(VERSION 2.6)

# Build libidp
file(GLOB srcs "*.cc")
add_library(idp SHARED ${srcs})

# Build test executable
file(GLOB test_srcs "test/*.cc")
googletest(test_idp
    "${test_srcs}" idp ${CMAKE_CURRENT_SOURCE_DIR}/test/data)

