# IDP L108 CMake configuration file
# Copyright 2011 Adam Greig & Jon Sowman

project(IDP CXX)
cmake_minimum_required(VERSION 2.6)

# Do not allow builds inside the source tree
if("${IDP_SOURCE_DIR}" STREQUAL "${IDP_BINARY_DIR}")
    message(FATAL_ERROR "Builds inside the source tree are not allowed.")
endif()

# Set debug/release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# Compiler flags
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Function to add a google test binary
function(googletest test_name test_sources test_libs test_data_root)
    # The executable itself, plus the gtest_main.cc test running
    add_executable(${test_name} ${test_sources})
    # Set libraries to be linked in
    target_link_libraries(${test_name} gmock ${test_libs})
    # Run the test command after building
    add_custom_command(TARGET ${test_name}
        POST_BUILD COMMAND ${test_name}
        ARGS "--test_data_root" "${test_data_root}" "--gtest_color=yes")
    # Add the test command to the global test runner (make test)
    add_test(${test_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name}
        "--test_data_root" "${test_data_root}" "--gtest_color=yes")
endfunction(googletest)

# Set include headers
include_directories(
    "${IDP_SOURCE_DIR}/lib/gmock/gmock/include"
    "${IDP_SOURCE_DIR}/lib/gmock/gmock/gtest/include"
    "${IDP_SOURCE_DIR}/src"
)

# Build required libraries
add_subdirectory("lib")

# Build IDP
add_subdirectory("src")

