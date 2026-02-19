#ifndef ROS_STRATEGY_H
#define ROS_STRATEGY_H

#include "Strategy.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Static trampoline for TX Task
static void rosTxTask(void* pvParameters);

class RosStrategy : public Strategy {
private:
    TaskHandle_t txTaskHandle;

public:
    RosStrategy(Robot& r) : Strategy(r), txTaskHandle(NULL) {}

    void setup() override {
        Serial.println("ROS Mode: Ready. Waiting for CMD:vL,vR or SERVO:id,angle");
        Serial.setTimeout(10); // Non-blocking read (10ms timeout)

        // Create the Odometry Publisher Task (Priority 2, higher than main loop)
        xTaskCreatePinnedToCore(
            rosTxTask,
            "RosTxTask",
            2048,
            this, // Pass 'this' instance
            2,
            &txTaskHandle,
            1
        );
    }

    void loop() override {
        // 1. RX: Poll Serial for Commands (Non-blocking, immediate execution)
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim(); // Remove whitespace/newlines

            // --- Motor Command: CMD:vL,vR ---
            if (input.startsWith("CMD:")) {
                int commaIndex = input.indexOf(',');
                if (commaIndex > 0) {
                    // Extract values after "CMD:"
                    float vL = input.substring(4, commaIndex).toFloat();
                    float vR = input.substring(commaIndex + 1).toFloat();

                    // Apply directly (m/s)
                    robot.chassis.setSpeeds(vL, vR);
                }
            }
            // --- Servo Command: SERVO:id,angle ---
            else if (input.startsWith("SERVO:")) {
                int firstComma = input.indexOf(',');
                if (firstComma > 0) {
                    int id = input.substring(6, firstComma).toInt();
                    float angle = input.substring(firstComma + 1).toFloat();

                    robot.arm.setAngle(id, angle);
                }
            }
        }

        // Yield slightly to prevent WDT, but keep it tight for fast RX
        vTaskDelay(1);
    }

    // Public method for the task to call
    void publishOdometry() {
        auto pos = robot.pose.lock();
        // Format: ODOM:x,y,theta
        Serial.printf("ODOM:%.3f,%.3f,%.3f\n", pos->x, pos->y, pos->theta);

        // Optional: Also publish Servo Feedback?
        // Might clog the line if too frequent. Let's stick to Odom for now.
    }
};

// Task Implementation
static void rosTxTask(void* pvParameters) {
    RosStrategy* strategy = static_cast<RosStrategy*>(pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50Hz

    while (1) {
        strategy->publishOdometry();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

#endif // ROS_STRATEGY_H
