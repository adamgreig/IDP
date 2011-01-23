# IDP L108 CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(idp CXX)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Build marslander
file(GLOB srcs "*.cc")
add_executable(idp "${srcs}")
#target_link_libraries(marslander marssimulation marsautopilot)

