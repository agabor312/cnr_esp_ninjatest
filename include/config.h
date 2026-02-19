#ifndef CONFIG_H
#define CONFIG_H

#include "servoHandler.h"
#include "motorHandler.h"

/**
 * --- Strategy Selection ---
 * Uncomment exactly ONE of the following lines to define the robot's brain.
 * STRATEGY_ROS: Control the robot via a serial interface (e.g. Raspberry Pi)
 * STRATEGY_AUTONOMOUS: Use a standalone state machine for Eurobot missions
 * STRATEGY_TEST: Diagnostic mode for hardware verification
 */
#define STRATEGY_ROS
// #define STRATEGY_AUTONOMOUS
// #define STRATEGY_TEST

/**
 * --- Servo Configuration ---
 * Define pins and protocol for the STS-series serial bus servos.
 */
#define SERVO_BUS_BAUD 1000000
#define SERVO_RX_PIN 16
// dont forget SERIAL_8N1
#define SERVO_TX_PIN 17
// Serial2.begin(SERVO_BUS_BAUD,SERIAL_8N1,RX_PIN, TX_PIN);
constexpr ServoModelConfig SERVO_CONFIG = ST3215_CONFIG;

/**
 * --- Robot Dimensions ---
 * TRACK_WIDTH is the horizontal distance between wheel centers.
 * tune TRACK_WIDTH for optimal performance
 */
constexpr float TRACK_WIDTH = 0.098;       // meters

/**
 * --- Motor Configuration ---
 * Assigns pins and acceleration profiles to the Left and Right motors.
 */
constexpr MotorConfig LEFT_MOTOR = DEFAULT_LEFT_MOTOR;
constexpr MotorConfig RIGHT_MOTOR = DEFAULT_RIGHT_MOTOR;

/**
 * --- Field Initialization ---
 * The starting coordinates of the robot on the Eurobot table.
 */
constexpr Position INITIAL_POSE = {0.0, 0.0, 0.0};

#endif // CONFIG_H
