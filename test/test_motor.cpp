/**
 * @file test_motor.cpp
 * @brief MotorHandler unit tests
 */

#include <unity.h>
#include "motorHandler.h"
#include "AccelStepper.h"
#include "utils.h"
#include "test_config.h"
#include "test_motor.h"

static MotorHandler* motorHandler = nullptr;
static RAII<Position>* position = nullptr;
static AccelStepper* mockMotorL = nullptr;
static AccelStepper* mockMotorR = nullptr;

static void motorSetUp(void) {
    position = new RAII<Position>();
    position->init(TestConfig::INITIAL_POSE);
    mockMotorL = new AccelStepper(1, 26, 25);
    mockMotorR = new AccelStepper(1, 33, 32);
    motorHandler = new MotorHandler(mockMotorL, mockMotorR);
    motorHandler->begin(position, TestConfig::TRACK_WIDTH, 
                        TestConfig::TEST_LEFT_MOTOR, TestConfig::TEST_RIGHT_MOTOR);
}

static void motorTearDown(void) {
    delete motorHandler;
    delete position;
    motorHandler = nullptr;
    position = nullptr;
    mockMotorL = nullptr;
    mockMotorR = nullptr;
}

static void test_turn_90_ccw(void) {
    motorHandler->turn(90.0f);
    TEST_ASSERT_LESS_THAN(0, mockMotorL->targetPosition());
    TEST_ASSERT_GREATER_THAN(0, mockMotorR->targetPosition());
}

static void test_turn_90_cw(void) {
    motorHandler->turn(-90.0f);
    TEST_ASSERT_GREATER_THAN(0, mockMotorL->targetPosition());
    TEST_ASSERT_LESS_THAN(0, mockMotorR->targetPosition());
}

static void test_state_velocity_mode(void) {
    TEST_ASSERT_FALSE(motorHandler->isRunning());
    motorHandler->setSpeeds(0.5f, 0.5f);
    TEST_ASSERT_TRUE(motorHandler->isRunning());
}

static void test_state_position_mode(void) {
    motorHandler->go(500);
    TEST_ASSERT_TRUE(motorHandler->isRunning());
}

void runMotorTests(void) {
    motorSetUp();
    RUN_TEST(test_turn_90_ccw);
    motorTearDown();
    
    motorSetUp();
    RUN_TEST(test_turn_90_cw);
    motorTearDown();
    
    motorSetUp();
    RUN_TEST(test_state_velocity_mode);
    motorTearDown();
    
    motorSetUp();
    RUN_TEST(test_state_position_mode);
    motorTearDown();
}
