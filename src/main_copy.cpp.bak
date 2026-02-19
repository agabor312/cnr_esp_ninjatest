#include <Arduino.h>
#include "config.h"
#include "servoHandler.h"
#include "motorHandler.h"
#include "utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- Hardware Configuration ---

#define MONITOR_BAUD 115200
#define SCAN_DELAY_MS 5000
#define MAX_SERVO_ID 20

// Global Objects
ServoHandler servoHandler;
ServoFeedback feedback;

MotorHandler motorHandler;
RAII<Position> robotPosition;

int i = 0;

void ping(){
    Serial.println("Scanning for servos...");
    bool servoFound = false;

    for (int id = 0; id <= MAX_SERVO_ID; ++id) {
        if (servoHandler.ping(id)) {
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

void odometer(){
    Serial.println("---------------------------");

    for (int i = 1; i<=6; i++) {
        Serial.println(servoHandler.getFeedback(i,feedback)? "Position "+ String(i)+ " : "+String(feedback.position_deg) : "could not get feedback");
    }
    Serial.println("---------------------------");
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void odometer2(){
    Serial.println("*********************************");

    for (int i = 5; i<=6; i++) {
        Serial.println("---------------------------");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Position "+ String(i)+ " : "+String(feedback.position_deg) +"°" : "could not get Position");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Speed "+ String(i)+ " : "+String(feedback.speed_rpm) + " RPM" : "could not get Speed");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Temperature "+ String(i)+ " : "+String(feedback.temperature_c) +" C" : "could not get Temperature");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Voltage "+ String(i)+ " : "+String(feedback.voltage) + " V" : "could not get Voltage");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Current "+ String(i)+ " : "+String(feedback.current_ma) + " mA" : "could not get Current");
        Serial.println(servoHandler.getFeedback(i,feedback)? "Moving? "+ String(i)+ " : "+String(feedback.is_moving): "could not get Moving?");
        Serial.println("---------------------------");

    }
    Serial.println("*********************************");
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void printRobotPosition() {
    auto pos = robotPosition.lock();
    Serial.printf("Robot Pose -> X: %.3f, Y: %.3f, Theta: %.3f\n", pos->x, pos->y, pos->theta);
}

void setup() {
    // Start the serial monitor for printing results
    Serial.begin(MONITOR_BAUD);
    while (!Serial) {
        ; // wait for serial port to connect.
    }
    Serial.println("\n--- Robot Initialization ---");

    // Initialize ThreadSafe Position
    if (!robotPosition.init({0.0f, 0.0f, 0.0f})) {
        Serial.println("Error: Failed to create Position Mutex!");
        while(1);
    }

    // Initialize Motor Handler
    // Only pass trackWidth, configs include wheelDiameter now
    if (!motorHandler.begin(&robotPosition, trackWidth, LEFT_MOTOR, RIGHT_MOTOR)) {
         Serial.println("Error: Failed to initialize MotorHandler!");
         while(1);
    }

    // // Start the hardware serial port for the servo bus
    // Serial2.begin(SERVO_BUS_BAUD,SERIAL_8N1,SERVO_RX_PIN, SERVO_TX_PIN);
    // // Note: 'handler' was renamed to 'servoHandler' for clarity
    // if (!servoHandler.begin(Serial2, SERVO_CONFIG)) {
    //     Serial.println("Error: Failed to initialize ServoHandler!");
    // }

    // // Disable torque on servos for testing
    // for (int i = 1; i<=6; i++) {
    //     servoHandler.setTorque(i, false);
    // }

    Serial.println("System Started.");



}

void loop() {
    // ping();
    // odometer2();
    if (i == 0) {
        motorHandler.turn(90);
        while(motorHandler.isRunning()){vTaskDelay(1);};
        motorHandler.go(100);
        while(motorHandler.isRunning()){vTaskDelay(1);};
    }
    //i++;
    i = (i + 1) % 4;
    // Print Odometry every second
    printRobotPosition();
    vTaskDelay(pdMS_TO_TICKS(1000));
}
