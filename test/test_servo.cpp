/**
 * @file test_servo.cpp
 * @brief ServoHandler unit tests
 */

#include <unity.h>
#include "servoHandler.h"
#include "STSServoDriver.h"
#include "test_config.h"
#include "test_servo.h"

static STSServoDriver* mockDriver = nullptr;
static ServoHandler* servoHandler = nullptr;
static HardwareSerial* dummySerial = nullptr;

static void servoSetUp(void) {
    mockDriver = new STSServoDriver();
    servoHandler = new ServoHandler(*mockDriver);
    dummySerial = new HardwareSerial();
    servoHandler->begin(*dummySerial, TestConfig::SERVO_CONFIG);
}

static void servoTearDown(void) {
    delete servoHandler;
    delete mockDriver;
    delete dummySerial;
    servoHandler = nullptr;
    mockDriver = nullptr;
    dummySerial = nullptr;
}

static void test_angle_to_position_zero(void) {
    servoHandler->setAngle(1, 0.0f);
    VirtualServo& servo = mockDriver->getServo(1);
    TEST_ASSERT_EQUAL(0, servo.target_position);
}

static void test_angle_to_position_half(void) {
    servoHandler->setAngle(1, 180.0f);
    VirtualServo& servo = mockDriver->getServo(1);
    TEST_ASSERT_INT_WITHIN(2, 2048, servo.target_position);
}

static void test_angle_to_position_max(void) {
    servoHandler->setAngle(1, 360.0f);
    VirtualServo& servo = mockDriver->getServo(1);
    TEST_ASSERT_EQUAL(4095, servo.target_position);
}

static void test_set_velocity(void) {
    servoHandler->setVelocity(3, 200);
    VirtualServo& servo = mockDriver->getServo(3);
    TEST_ASSERT_EQUAL(STSMode::VELOCITY, servo.mode);
    TEST_ASSERT_INT_WITHIN(1, 833, servo.target_velocity);
}

static void test_stop(void) {
    servoHandler->setVelocity(3, 200);
    servoHandler->stop(3);
    VirtualServo& servo = mockDriver->getServo(3);
    TEST_ASSERT_EQUAL(0, servo.target_velocity);
}

static void test_get_angle_valid(void) {
    mockDriver->getServo(2).current_position = 1024;
    float angle = servoHandler->getAngle(2);
    TEST_ASSERT_FLOAT_WITHIN(TestConfig::SERVO_TOLERANCE, 90.0f, angle);
}

static void test_get_feedback_all_fields(void) {
    VirtualServo& servo = mockDriver->getServo(4);
    servo.current_position = 2048;
    servo.current_speed = 417;
    servo.current_temperature = 45;
    servo.current_voltage_raw = 123;
    servo.current_current_amps = 0.5f;
    servo.is_moving_status = true;
    
    ServoFeedback feedback;
    bool success = servoHandler->getFeedback(4, feedback);
    
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_FLOAT_WITHIN(TestConfig::SERVO_TOLERANCE, 180.0f, feedback.position_deg);
}

void runServoTests(void) {
    servoSetUp();
    RUN_TEST(test_angle_to_position_zero);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_angle_to_position_half);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_angle_to_position_max);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_set_velocity);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_stop);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_get_angle_valid);
    servoTearDown();
    
    servoSetUp();
    RUN_TEST(test_get_feedback_all_fields);
    servoTearDown();
}
