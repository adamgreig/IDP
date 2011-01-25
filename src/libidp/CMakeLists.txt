# IDP L108 CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(libidp CXX)
cmake_minimum_required(VERSION 2.6)

# Build libidp
file(GLOB srcs "*.cc")
add_library(idp STATIC ${srcs})

target_link_libraries(idp "${IDP_SOURCE_DIR}/lib/librobot/librobot.a")

# Build test executable
file(GLOB test_srcs "test/*.cc")
googletest(test_idp
    "${test_srcs}" idp ${CMAKE_CURRENT_SOURCE_DIR}/test/data)

