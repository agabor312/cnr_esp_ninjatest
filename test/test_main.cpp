/**
 * @file test_main.cpp
 * @brief Main test runner - includes all test modules
 * 
 * Structure:
 * - Each component has its own test_*.cpp file with focused tests
 * - This file serves as the unified entry point
 * - Run with: pio test
 */

#include <unity.h>
#include "Arduino.h"

// Include test module headers
#include "test_servo.h"
#include "test_motor.h"
#include "test_robot.h"
#include "test_strategy.h"
#include "test_utils.h"

// Global Serial objects for mocks
HardwareSerial Serial;
HardwareSerial Serial2;

void setUp(void) {
    // Global setup - runs before each test
}

void tearDown(void) {
    // Global cleanup - runs after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Run all test suites
    runServoTests();
    runMotorTests();
    runRobotTests();
    runStrategyTests();
    runUtilsTests();
    
    return UNITY_END();
}
