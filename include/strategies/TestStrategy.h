#ifndef TEST_STRATEGY_H
#define TEST_STRATEGY_H

#include "Strategy.h"

// Hardware Test Strategy: Validates motors and servos
class TestStrategy : public Strategy {
public:
    // Constructor passes robot reference to the base class
    TestStrategy(Robot& r) : Strategy(r) {}

    void setup() override {
        Serial.println("--- HARDWARE TEST MODE ---");
        
        // 1. Check Servos
        Serial.println("Checking Servos...");
        for (int i = 1; i <= 6; i++) {
            if (robot.arm.ping(i)) {
                Serial.printf("Servo ID %d: OK\n", i);
            } else {
                Serial.printf("Servo ID %d: FAIL\n", i);
            }
        }
        
        // 2. Initial Motor Move
        Serial.println("Testing Motors: Forward 1000 steps...");
        robot.chassis.go(1000);
    }

    void loop() override {
        static unsigned long lastCheck = 0;
        
        // Every 2 seconds, print status
        if (millis() - lastCheck > 2000) {
            if (robot.chassis.isRunning()) {
                Serial.println("Status: Motors Running...");
            } else {
                Serial.println("Status: Idle. Turning in place...");
                robot.chassis.turn(360); // Spin test
            }
            lastCheck = millis();
        }
        
        // Also verify servo telemetry
        ServoFeedback fb;
        if (robot.arm.getFeedback(1, fb)) {
            // Check voltage
            if (fb.voltage < 10.0) {
                Serial.printf("WARNING: Low Voltage on Servo 1: %.2fV\n", fb.voltage);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
};

#endif // TEST_STRATEGY_H