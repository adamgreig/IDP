# IDP L108 CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(IDPsrcs CXX)
cmake_minimum_required(VERSION 2.6)

# Compiler flags for IDP code
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Build libidp
add_subdirectory("libidp")

# Build idp
add_subdirectory("idpbin")

