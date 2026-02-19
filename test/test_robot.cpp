/**
 * @file test_robot.cpp
 * @brief Robot unit tests
 */

#include <unity.h>
#include "robot.h"
#include "test_config.h"
#include "test_robot.h"

static Robot* robot = nullptr;

static void robotSetUp(void) {
    robot = new Robot();
    robot->pose.init(TestConfig::INITIAL_POSE);
}

static void robotTearDown(void) {
    delete robot;
    robot = nullptr;
}

static void test_creation(void) {
    TEST_ASSERT_NOT_NULL(robot);
}

static void test_motors_accessible(void) {
    robot->motors.begin(&robot->pose, TestConfig::TRACK_WIDTH,
                        TestConfig::TEST_LEFT_MOTOR, TestConfig::TEST_RIGHT_MOTOR);
    TEST_ASSERT_NOT_NULL(&robot->motors);
}

static void test_pose_initialization(void) {
    auto pos = robot->pose.lock();
    TEST_ASSERT_FLOAT_WITHIN(TestConfig::FLOAT_TOLERANCE, 0.0f, pos->x);
}

void runRobotTests(void) {
    robotSetUp();
    RUN_TEST(test_creation);
    robotTearDown();
    
    robotSetUp();
    RUN_TEST(test_motors_accessible);
    robotTearDown();
    
    robotSetUp();
    RUN_TEST(test_pose_initialization);
    robotTearDown();
}
