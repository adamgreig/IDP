// IDP Test Suite
// Copyright 2011 Adam Greig & Jon Sowman
//
// test_mission_supervisor.cc
// Unit tests for the Mission Supervisor class

#include <gtest/gtest.h>
#include <string>
#include "../mission_supervisor.h"

extern std::string TEST_DATA_ROOT;

class TestMissionSupervisor : public ::testing::Test
{
    public:
        TestMissionSupervisor()
        {
        }
};

TEST_F(TestMissionSupervisor, Pass)
{
    // just pass
    EXPECT_EQ(1, 1);
}
