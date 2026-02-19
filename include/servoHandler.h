#ifndef SERVOHANDLER_H
#define SERVOHANDLER_H

#include "Arduino.h"
#include <STSServoDriver.h>
#include "freertos/semphr.h"

/**
 * @brief Comprehensive feedback data from a Feetech STS servo.
 */
struct ServoFeedback {
    float position_deg; // Current angle in degrees
    int speed_rpm;      // Current speed in RPM
    int temperature_c;  // Internal temperature in Celsius
    float voltage;      // Operating voltage [Volts]
    float current_ma;   // Power consumption [milliamps]
    bool is_moving;     // Whether the servo is currently seeking a target
};

/**
 * @brief Physical and register characteristics of a specific servo model.
 */
struct ServoModelConfig {
    float rpm_per_unit;
    int max_position;
    int max_torque_val;
    float max_angle_deg;

    float voltage_divisor;          // Divisor for raw register value
    float current_multiplier_ma;    // Multiplier for Amps to mA

    // Register Addresses for the STS protocol
    byte reg_torque_switch;
    byte reg_torque_limit;
    byte reg_voltage;
};

/**
 * @brief Configuration for the Feetech ST3215 servo used in ECN Nantrobot.
 */
constexpr ServoModelConfig ST3215_CONFIG = {
    .rpm_per_unit = 0.24f,
    .max_position = 4095,
    .max_torque_val = 1000,
    .max_angle_deg = 360.0f,
    .voltage_divisor = 10.0f,
    .current_multiplier_ma = 1000.0f,
    .reg_torque_switch = 0x28,
    .reg_torque_limit = 0x30,
    .reg_voltage = 0x3E
};

/**
 * @brief Thread-safe manager for serial bus servos.
 * 
 * This class uses a Mutex to ensure that multiple strategies or tasks 
 * do not conflict when sending commands over the shared half-duplex serial bus.
 */
class ServoHandler {
public:
    // --- Constructors ---
    ServoHandler();
    ServoHandler(STSServoDriver& driver_for_testing);

    /**
     * @brief Initializes the serial bus and the access mutex.
     * @param serial The hardware serial port (e.g. Serial2).
     * @param config The physical model configuration (defaults to ST3215).
     */
    bool begin(HardwareSerial& serial, const ServoModelConfig& config = ST3215_CONFIG);

    // --- High-Level Motion Control ---

    /**
     * @brief Sets the target angle for a servo.
     * @param servoId ID of the servo on the bus.
     * @param angle_deg Target angle in degrees.
     */
    bool setAngle(byte servoId, float angle_deg);

    /**
     * @brief Sets target angle and specific speed.
     */
    bool setAngle(byte servoId, float angle_deg, int speed_rpm);

    /**
     * @brief Continuous velocity control (e.g. for wheels or continuous rotation).
     */
    bool setVelocity(byte servoId, int speed_rpm);

    /**
     * @brief Stops the servo at its current position.
     */
    bool stop(byte servoId);

    // --- Torque Control ---

    /**
     * @brief Enables or disables motor power (torque).
     * Useful for manual movement or safety.
     */
    bool setTorque(byte servoId, bool enable);

    /**
     * @brief Sets the maximum torque percentage (0-100%).
     */
    bool setTorqueLimit(byte servoId, float percentage);

    // --- Feedback ---

    /**
     * @brief Retrieves the current position in degrees.
     */
    float getAngle(byte servoId);

    /**
     * @brief Retrieves full diagnostic telemetry from the servo.
     */
    bool getFeedback(byte servoId, ServoFeedback& feedback);

    /**
     * @brief Pings a servo ID to check if it's alive on the bus.
     */
    bool ping(byte servoId);

private:
    STSServoDriver internal_driver; 
    STSServoDriver& sts;            

    SemaphoreHandle_t busMutex; // Protects the physical Serial line
    ServoModelConfig config; 

    // Unit conversion helpers
    int angleToPosition(float angle_deg);
    float positionToAngle(int position);
    int rpmToSpeed(int rpm);
    int speedToRpm(int speed_val);
};

#endif // SERVOHANDLER_H