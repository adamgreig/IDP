# IDP CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(idpbin CXX)
cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Build idp
file(GLOB srcs "*.cc")
add_executable(idpbin ${srcs})
target_link_libraries(idpbin idp)

