/**
 * @file test_strategy.cpp
 * @brief Strategy interface unit tests
 */

#include <unity.h>
#include "strategies/Strategy.h"
#include "TestStrategy.h"
#include "robot.h"
#include "test_config.h"
#include "test_strategy.h"

static TestStrategy* testStrategy = nullptr;
static Strategy* strategyPtr = nullptr;
static Robot* robot = nullptr;

static void strategySetUp(void) {
    robot = new Robot();
    robot->pose.init(TestConfig::INITIAL_POSE);
    testStrategy = new TestStrategy(*robot);
    strategyPtr = testStrategy;
}

static void strategyTearDown(void) {
    delete testStrategy;
    delete robot;
    testStrategy = nullptr;
    strategyPtr = nullptr;
    robot = nullptr;
}

static void test_setup_called(void) {
    TEST_ASSERT_EQUAL(0, testStrategy->setupCallCount);
    testStrategy->setup();
    TEST_ASSERT_EQUAL(1, testStrategy->setupCallCount);
}

static void test_loop_called(void) {
    testStrategy->loop();
    TEST_ASSERT_EQUAL(1, testStrategy->loopCallCount);
}

static void test_polymorphism(void) {
    strategyPtr->setup();
    TEST_ASSERT_EQUAL(1, testStrategy->setupCallCount);
}

void runStrategyTests(void) {
    strategySetUp();
    RUN_TEST(test_setup_called);
    strategyTearDown();
    
    strategySetUp();
    RUN_TEST(test_loop_called);
    strategyTearDown();
    
    strategySetUp();
    RUN_TEST(test_polymorphism);
    strategyTearDown();
}
