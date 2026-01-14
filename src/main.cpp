#include <Arduino.h>
#include "servoHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- Hardware Configuration ---
#define SERVO_BUS_BAUD 1000000
#define MONITOR_BAUD 115200
#define RX_PIN 16
#define TX_PIN 17
#define SCAN_DELAY_MS 5000
#define MAX_SERVO_ID 20

ServoHandler handler;

void setup() {
    // Start the serial monitor for printing results
    Serial.begin(MONITOR_BAUD);
    while (!Serial) {
        ; // wait for serial port to connect.
    }
    Serial.println("\n--- Servo Scanner Initialized ---");

    // Start the hardware serial port for the servo bus
    Serial2.begin(SERVO_BUS_BAUD, RX_PIN, TX_PIN);
    handler.begin(Serial2);

    Serial.println("Starting first scan...");
}

void loop() {
    Serial.println("Scanning for servos...");
    bool servoFound = false;

    for (int id = 0; id <= MAX_SERVO_ID; ++id) {
        if (handler.ping(id)) {
            Serial.print("Response from servo ID: ");
            Serial.println(id);
            servoFound = true;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay between pings
    }

    if (!servoFound) {
        Serial.println("No servos found in ID range 0-20.");
    }

    Serial.print("Scan complete. Waiting ");
    Serial.print(SCAN_DELAY_MS / 1000);
    Serial.println(" seconds before next scan.\n");
    vTaskDelay(pdMS_TO_TICKS(SCAN_DELAY_MS));
}
