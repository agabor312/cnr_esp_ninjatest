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
ServoFeedback feedback;

void ping(){
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

void odometer(){
    Serial.println("---------------------------");

    for (int i = 1; i<=6; i++) {
        Serial.println(handler.getFeedback(i,feedback)? "Position "+ String(i)+ " : "+String(feedback.position_deg) : "could not get feedback");
    }
    Serial.println("---------------------------");
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void odometer2(){
    Serial.println("*********************************");

    for (int i = 5; i<=6; i++) {
        Serial.println("---------------------------");
        Serial.println(handler.getFeedback(i,feedback)? "Position "+ String(i)+ " : "+String(feedback.position_deg) +"°" : "could not get Position");
        Serial.println(handler.getFeedback(i,feedback)? "Speed "+ String(i)+ " : "+String(feedback.speed_rpm) + " RPM" : "could not get Speed");
        Serial.println(handler.getFeedback(i,feedback)? "Temperature "+ String(i)+ " : "+String(feedback.temperature_c) +" C" : "could not get Temperature");
        Serial.println(handler.getFeedback(i,feedback)? "Voltage "+ String(i)+ " : "+String(feedback.voltage) + " V" : "could not get Voltage");
        Serial.println(handler.getFeedback(i,feedback)? "Current "+ String(i)+ " : "+String(feedback.current_ma) + " mA" : "could not get Current");
        Serial.println(handler.getFeedback(i,feedback)? "Moving? "+ String(i)+ " : "+String(feedback.is_moving): "could not get Moving?");
        Serial.println("---------------------------");

    }
    Serial.println("*********************************");
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void setup() {
    // Start the serial monitor for printing results
    Serial.begin(MONITOR_BAUD);
    while (!Serial) {
        ; // wait for serial port to connect.
    }
    Serial.println("\n--- Servo Tests Initialized ---");

    // Start the hardware serial port for the servo bus
    Serial2.begin(SERVO_BUS_BAUD,SERIAL_8N1,RX_PIN, TX_PIN);
    handler.begin(Serial2);

    for (int i = 1; i<=6; i++) {
        handler.setTorque(i, false);
    }


    Serial.println("Starting ...");
}

void loop() {
    //ping();
    //odometer();
    odometer2();
}
