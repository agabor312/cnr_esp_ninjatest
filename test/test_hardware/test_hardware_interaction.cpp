#include <Arduino.h>
#include <unity.h>
#include "servoHandler.h"
#include "freertos/FreeRTOS.h" // For the pdMS_TO_TICKS macro
#include "freertos/task.h"     // For vTaskDelay

// --- Hardware Configuration ---
// IMPORTANT: You must change these values to match your physical setup.
#define SERVO_ID 5
#define SERIAL_BAUD 1000000
#define RX_PIN 16
#define TX_PIN 17

ServoHandler handler;
ServoFeedback feedback; // Re-use this struct in tests

void setUp(void) {
    // This is run before each test
    // NOTE: Serial2 is often on pins 16 (RX) and 17 (TX) for standard ESP32 boards.
    Serial2.begin(SERIAL_BAUD,SERIAL_8N1,RX_PIN,TX_PIN);
    handler.begin(Serial2);
    vTaskDelay(pdMS_TO_TICKS(500)); // Give the bus a moment to settle
}

void tearDown(void) {
    // Ensure the servo is stopped and torque is re-enabled after each test for safety.
    handler.stop(SERVO_ID);
    handler.setTorque(SERVO_ID, true);
    vTaskDelay(pdMS_TO_TICKS(50));
}

// --- Test Cases ---

void test_servo_responds_to_ping(void) {
    bool ping_success = handler.ping(SERVO_ID);
    TEST_ASSERT_TRUE_MESSAGE(ping_success, "Failed to ping the servo. Check wiring, power, and ID.");
}

void test_set_and_get_angle_feedback(void) {
    bool set_success = handler.setAngle(SERVO_ID, 90.0f);
    TEST_ASSERT_TRUE_MESSAGE(set_success, "Failed to send setAngle command.");
    vTaskDelay(pdMS_TO_TICKS(1500)); // Wait for the servo to physically move.
    float reported_angle = handler.getAngle(SERVO_ID);
    TEST_ASSERT_GREATER_THAN_MESSAGE(-1.0, reported_angle, "Failed to get a valid angle from the servo after commanding a move.");
    TEST_ASSERT_FLOAT_WITHIN(2.0, 90.0, reported_angle);
}

void test_torque_control_visual_check(void) {
    handler.setAngle(SERVO_ID, 180.0f);
    vTaskDelay(pdMS_TO_TICKS(1500));

    handler.setTorque(SERVO_ID, false);
    TEST_MESSAGE("Torque is OFF. Try moving the servo by hand. It should move easily.");
    vTaskDelay(pdMS_TO_TICKS(5000));

    handler.setTorque(SERVO_ID, true);
    TEST_MESSAGE("Torque is ON. Try moving the servo by hand. It should resist.");
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void test_velocity_command_and_feedback(void) {
    int commanded_rpm = 60;
    bool vel_success = handler.setVelocity(SERVO_ID, commanded_rpm);
    TEST_ASSERT_TRUE_MESSAGE(vel_success, "Failed to send setVelocity command.");

    vTaskDelay(pdMS_TO_TICKS(500)); // Wait a moment for the servo to get up to speed

    bool feedback_success = handler.getFeedback(SERVO_ID, feedback);
    TEST_ASSERT_TRUE(feedback_success);
    TEST_ASSERT_INT_WITHIN(5, commanded_rpm, feedback.speed_rpm);

    bool stop_success = handler.stop(SERVO_ID);
    TEST_ASSERT_TRUE_MESSAGE(stop_success, "Failed to send stop command.");
    vTaskDelay(pdMS_TO_TICKS(500));

    handler.getFeedback(SERVO_ID, feedback);
    TEST_ASSERT_INT_WITHIN(2, 0, feedback.speed_rpm);
}

void test_get_feedback_returns_plausible_values(void) {
    bool feedback_success = handler.getFeedback(SERVO_ID, feedback);
    TEST_ASSERT_TRUE_MESSAGE(feedback_success, "Failed to retrieve feedback packet.");

    TEST_ASSERT_FLOAT_WITHIN(360.0, 180.0, feedback.position_deg);
    TEST_ASSERT_INT_WITHIN(500, 0, feedback.speed_rpm);
    TEST_ASSERT_INT_WITHIN(100, 0, feedback.temperature_c);
    TEST_ASSERT_FLOAT_WITHIN(15.0, 5.0, feedback.voltage);
    TEST_ASSERT_FLOAT_WITHIN(2000.0, 0.0, feedback.current_ma);
}


void setup() {
    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();
    RUN_TEST(test_servo_responds_to_ping);
    RUN_TEST(test_set_and_get_angle_feedback);
    RUN_TEST(test_get_feedback_returns_plausible_values);
    RUN_TEST(test_velocity_command_and_feedback);
    RUN_TEST(test_torque_control_visual_check); // Run the interactive test last
    UNITY_END();
}

void loop() {
}
