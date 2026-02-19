#ifndef AUTONOMOUS_STRATEGY_H
#define AUTONOMOUS_STRATEGY_H

#include "Strategy.h"

// Example Autonomous Strategy: Move in a square
class AutonomousStrategy : public Strategy {
private:
    enum State { IDLE, MOVING, TURNING, WAITING };
    State currentState = IDLE;
    unsigned long timer = 0;
    int movesCount = 0;

public:
    // Constructor passes robot reference to the base class
    AutonomousStrategy(Robot& r) : Strategy(r) {}

    void setup() override {
        Serial.println("Auto Mode: Starting Mission!");
        currentState = IDLE;
        timer = millis();
    }

    void loop() override {
        switch(currentState) {
            case IDLE:
                if (millis() - timer > 2000) {
                    Serial.println("Moving forward...");
                    robot.chassis.go(500); // 500 steps
                    currentState = MOVING;
                }
                break;
                
            case MOVING:
                if (!robot.chassis.isRunning()) {
                    Serial.println("Turning 90 degrees...");
                    robot.chassis.turn(90);
                    currentState = TURNING;
                }
                break;
                
            case TURNING:
                if (!robot.chassis.isRunning()) {
                    movesCount++;
                    if (movesCount < 4) {
                        Serial.println("Next leg...");
                        robot.chassis.go(500);
                        currentState = MOVING;
                    } else {
                        Serial.println("Square complete! Waiting...");
                        currentState = WAITING;
                        timer = millis();
                    }
                }
                break;
                
            case WAITING:
                if (millis() - timer > 5000) {
                    movesCount = 0;
                    currentState = IDLE;
                    timer = millis();
                }
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Run logic at 10Hz
    }
};

#endif // AUTONOMOUS_STRATEGY_H