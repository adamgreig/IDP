# IDP CMake Configuration File
# Copyright 2011 Adam Greig & Jon Sowman

project(gmock CXX)
cmake_minimum_required(VERSION 2.6)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/gtest"
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/gtest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock"
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/include"
)

set(srcs
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/gtest/src/gtest-all.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/src/gmock-all.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/gmock/gtest/src/gtest_main.cc"
)

add_library(gmock STATIC ${srcs})
target_link_libraries(gmock pthread)
