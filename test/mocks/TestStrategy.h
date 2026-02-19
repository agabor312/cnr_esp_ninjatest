/**
 * @file TestStrategy.h
 * @brief Test implementation of Strategy interface for unit testing
 * 
 * This strategy tracks how many times setup() and loop() are called,
 * and provides a way to verify the Strategy interface mechanics.
 */

#ifndef TEST_STRATEGY_H
#define TEST_STRATEGY_H

#include "strategies/Strategy.h"

class TestStrategy : public Strategy {
public:
    // Counters for tracking calls
    int setupCallCount = 0;
    int loopCallCount = 0;
    
    // Flag to verify robot reference is accessible
    bool canAccessRobot = false;
    bool canAccessMotors = false;
    bool canAccessServos = false;
    bool canAccessPose = false;
    
    // Constructor
    TestStrategy(Robot& r) : Strategy(r) {}
    
    void setup() override {
        setupCallCount++;
        
        // Verify robot reference is accessible
        canAccessRobot = true;
        
        // Try to access robot components
        // Note: Robot members are public, so this should work
        canAccessMotors = (&robot.motors != nullptr);
        canAccessServos = (&robot.servos != nullptr);
        canAccessPose = (&robot.pose != nullptr);
    }
    
    void loop() override {
        loopCallCount++;
        
        // In a real implementation, this would do work
        // For testing, we just track the call
    }
    
    // Helper to reset counters
    void reset() {
        setupCallCount = 0;
        loopCallCount = 0;
        canAccessRobot = false;
        canAccessMotors = false;
        canAccessServos = false;
        canAccessPose = false;
    }
};

#endif // TEST_STRATEGY_H
