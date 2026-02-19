/**
 * @file test_config.h
 * @brief Test configuration constants for unit testing
 * 
 * This file provides standardized test values to avoid magic numbers
 * and ensure consistent test data across all test files.
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include "motorHandler.h"
#include "servoHandler.h"

namespace TestConfig {

    // ============================================================================
    // Robot Physical Dimensions
    // ============================================================================
    
    constexpr float TRACK_WIDTH = 0.095f;           // Distance between wheels [m]
    constexpr float WHEEL_DIAMETER = 0.0675f;       // Wheel diameter [m]
    constexpr float WHEEL_RADIUS = WHEEL_DIAMETER / 2.0f;
    
    // ============================================================================
    // Servo Configuration
    // ============================================================================
    
    constexpr ServoModelConfig SERVO_CONFIG = ST3215_CONFIG;
    
    // ============================================================================
    // Motor Configuration
    // ============================================================================
    
    constexpr int STEPS_PER_REVOLUTION = 200;
    constexpr float MAX_SPEED = 20000.0f;
    constexpr float ACCELERATION = 20000.0f / 32.0f;
    
    // Test motor configs with consistent parameters
    constexpr MotorConfig TEST_LEFT_MOTOR = {
        .motorInterfaceType = 1,
        .step_pin = 26,
        .dir_pin = 25,
        .direction_invert = true,
        .step_invert = false,
        .enable_invert = false,
        .max_speed = MAX_SPEED,
        .acceleration = ACCELERATION,
        .steps_per_revolution = STEPS_PER_REVOLUTION,
        .wheelDiameter = WHEEL_DIAMETER
    };
    
    constexpr MotorConfig TEST_RIGHT_MOTOR = {
        .motorInterfaceType = 1,
        .step_pin = 33,
        .dir_pin = 32,
        .direction_invert = false,
        .step_invert = false,
        .enable_invert = false,
        .max_speed = MAX_SPEED,
        .acceleration = ACCELERATION,
        .steps_per_revolution = STEPS_PER_REVOLUTION,
        .wheelDiameter = WHEEL_DIAMETER
    };
    
    // Expected motor constants (calculated)
    // metersPerStep = (2 * PI / steps) * (diameter / 2)
    constexpr double EXPECTED_METERS_PER_STEP = 0.001060290203f;  // ≈ π × 0.0675 / 200
    constexpr float EXPECTED_STEP_DISTANCE = 0.001060290203f;     // Same formula
    
    // ============================================================================
    // Servo Configuration
    // ============================================================================
    
    // ST3215 constants from servoHandler.h
    constexpr float MAX_SERVO_ANGLE = 360.0f;
    constexpr int MAX_SERVO_POSITION = 4095;
    constexpr float RPM_PER_UNIT = 0.24f;
    
    // Expected conversion values
    constexpr int EXPECTED_POSITION_90DEG = 1024;   // 90/360 * 4095 ≈ 1023.75
    constexpr int EXPECTED_POSITION_180DEG = 2048;  // 180/360 * 4095 = 2047.5
    constexpr int EXPECTED_SPEED_100RPM = 417;      // 100 / 0.24 ≈ 416.67
    constexpr int EXPECTED_SPEED_240RPM = 1000;     // 240 / 0.24 = 1000
    
    // ============================================================================
    // Position Test Data
    // ============================================================================
    
    constexpr Position INITIAL_POSE = {0.0f, 0.0f, 0.0f};
    constexpr Position TEST_POSE_1 = {1.0f, 2.0f, 0.5f};
    
    // ============================================================================
    // Test Tolerances
    // ============================================================================
    
    constexpr float FLOAT_TOLERANCE = 0.001f;       // 1mm for position
    constexpr float ANGLE_TOLERANCE = 0.01f;        // 0.01 radians
    constexpr float SERVO_TOLERANCE = 0.1f;         // For servo conversions
    
    // ============================================================================
    // Movement Test Values
    // ============================================================================
    
    constexpr int TEST_DISTANCE_STEPS = 1000;
    constexpr float TEST_TURN_ANGLE = 90.0f;        // degrees
    constexpr float TEST_SPEED_MPS = 0.5f;          // m/s
    
    // Arc length for 90° turn with TRACK_WIDTH
    // arc = (angle_rad * trackWidth) / 2
    constexpr float EXPECTED_ARC_LENGTH_90DEG = 0.074611553f;  // (π/2 × 0.095) / 2
    
} // namespace TestConfig

#endif // TEST_CONFIG_H
