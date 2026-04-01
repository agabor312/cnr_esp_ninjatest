#ifndef ATTEMPT1_H
#define ATTEMPT1_H

#include "Strategy.h"

// Hardware Test Strategy: Validates motors and servos
class Attempt_1 : public Strategy {
public:
    // Constructor passes robot reference to the base class
    Attempt_1(Robot& r) : Strategy(r) {}

    int mmtostep(int mm)
    {
        int steps = mm * 1;
        return steps;
    }
    void setup() override {
        Serial.println("Ninja Attempt");
        
        //Testing code optimization options
        Serial.println(sizeof(robot.servos));

        // 1. Check Servos
        Serial.println("Checking Servos...");
        for (int i = 1; i <= 6; i++) {
            if (robot.servos.ping(i)) {
                Serial.printf("Servo ID %d: OK\n", i);
            } else {
                Serial.printf("Servo ID %d: FAIL\n", i);
            }
        }
        
        // 2. Initial Motor Move
        Serial.println("Testing Motors: Forward 1000 steps...");
        robot.motors.go(1000);
    }

    void loop() override {
        static unsigned long lastCheck = 0;
        
        // Every 2 seconds, print status
        if (millis() - lastCheck > 2000) {
            if (robot.motors.isRunning()) {
                Serial.println("Status: Motors Running...");
            } else {
                Serial.println("Status: Idle. Turning in place...");
                robot.motors.turn(360); // Spin test
            }
            lastCheck = millis();
        }
        
        // Also verify servo telemetry
        ServoFeedback fb;
        if (robot.servos.getFeedback(1, fb)) {
            // Check voltage
            if (fb.voltage < 10.0) {
                Serial.printf("WARNING: Low Voltage on Servo 1: %.2fV\n", fb.voltage);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
        if (false) {
            //the true shenanigans assumption in this stupid code is 1 step per mm
            robot.motors.go(mmtostep(1350));
            robot.motors.turn(-90);
            
        }
    }
};

#endif // Attempt1_H