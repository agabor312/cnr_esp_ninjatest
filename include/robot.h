#ifndef ROBOT_H
#define ROBOT_H

#include "motorHandler.h"
#include "servoHandler.h"
#include "utils.h"
#include "config.h"

/**
 * @brief Main Hardware Abstraction Layer (HAL) class for the ECN Nantrobot.
 * Centralizes access to all physical robot systems (Motors, Servos, Odometry).
 */
class Robot {
public:

    MotorHandler motors; // Handles stepper motors and high-level movement
    ServoHandler servos; // Handles communication with the serial bus servos
    RAII<Position> pose; // Thread-safe field coordinates (x, y, theta)

    /**
     * @brief Initializes all hardware components of the robot.
     * @return true if all initializations succeeded.
     */
    bool begin() {
        return pose.init(INITIAL_POSE))
        && motors.begin(&pose, TRACK_WIDTH, LEFT_MOTOR, RIGHT_MOTOR))
        && Serial2.begin(SERVO_BUS_BAUD, SERIAL_8N1, SERVO_RX_PIN, SERVO_TX_PIN)
        && servos.begin(Serial2, SERVO_CONFIG));
    }
};

#endif // ROBOT_H