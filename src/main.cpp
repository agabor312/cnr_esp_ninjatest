#include <Arduino.h>
#include "config.h"
#include "robot.h"

// Strategy Includes: Only the selected strategy is compiled to save memory.
#ifdef STRATEGY_ROS
    #include "strategies/RosStrategy.h"
#elif defined(STRATEGY_AUTONOMOUS)
    #include "strategies/AutonomousStrategy.h"
#elif defined(STRATEGY_TEST)
    #include "strategies/TestStrategy.h"
#elif defined(ATTEMPT1)
    #include "strategies/Attempt1.h"
#endif

// --- Global System Objects ---
Robot robot;                    // The physical robot hardware (HAL)
Strategy* activeStrategy = nullptr; // Pointer to the logic currently in control

/**
 * @brief Standard Arduino setup function.
 * Initializes hardware and instantiates the selected strategy.
 */
void setup() {
    // Start serial for debugging
    Serial.begin(115200);
    while (!Serial && millis() < 2000); // Wait briefly for Serial to connect

    Serial.println("\n--- Starting Robot System ---");

    // 1. Initialize Robot Hardware (HAL)
    // This starts background tasks for motors and configures the servo bus.
    if (!robot.begin()) {
        Serial.println("Critical Error: Robot hardware initialization failed!");
    }

    // 2. Strategy Factory: Instantiates the "Brain" selected in config.h
    #ifdef STRATEGY_ROS
        activeStrategy = new RosStrategy(robot);
    #elif defined(STRATEGY_AUTONOMOUS)
        activeStrategy = new AutonomousStrategy(robot);
    #elif defined(STRATEGY_TEST)
        activeStrategy = new TestStrategy(robot);
    #elif defined(ATTEMPT1)
        activeStrategy = new Attempt_1(robot);
    #endif

    if (activeStrategy != nullptr) {
        activeStrategy->setup();
    } else {
        Serial.println("Error: No Strategy defined!");
    }
}

/**
 * @brief Standard Arduino loop function.
 * Delegates all high-level logic to the active strategy.
 */
void loop() {
    if (activeStrategy != nullptr) {
        activeStrategy->loop();
    } else {
        // Fallback if no strategy is active
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
