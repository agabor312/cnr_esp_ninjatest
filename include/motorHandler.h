#ifndef MOTORHANDLER_H
#define MOTORHANDLER_H
#include "Arduino.h"
#include <AccelStepper.h>
#include "utils.h"

/**
 * @brief High-level states for the robot's movement logic.
 */
enum MotorState {
    IDLE,           // Motors are stopped and holding position
    VELOCITY_MODE,  // Constant speed control (usually from ROS)
    POSITION_MODE   // Target-based movement (e.g. "go 1000 steps")
};

/**
 * @brief Configuration parameters for an individual stepper motor.
 */
struct MotorConfig {
    int motorInterfaceType;
    int step_pin;
    int dir_pin;
    bool direction_invert;
    bool step_invert;
    bool enable_invert;
    float max_speed;
    float acceleration;
    int steps_per_revolution;
    float wheelDiameter; // [meters]
};

// Default hardware configurations for the ECN Nantrobot
constexpr MotorConfig DEFAULT_LEFT_MOTOR = {
    .motorInterfaceType = 1,
    .step_pin = 26,
    .dir_pin = 25,
    .direction_invert = true,
    .step_invert = false,
    .enable_invert = false,
    .max_speed = 20000,
    .acceleration = 20000.0/32.0,
    .steps_per_revolution = 200,
    .wheelDiameter = 0.0675
};

constexpr MotorConfig DEFAULT_RIGHT_MOTOR = {
    .motorInterfaceType = 1,
    .step_pin = 33,
    .dir_pin = 32,
    .direction_invert = false,
    .step_invert = false,
    .enable_invert = false,
    .max_speed = 20000,
    .acceleration = 20000.0/32.0,
    .steps_per_revolution = 200,
    .wheelDiameter = 0.0675
};

/**
 * @brief Represents the 2D pose of the robot on the field.
 */
struct Position {
    float x;        // [meters]
    float y;        // [meters]
    float theta;    // [radians]
};

/**
 * @brief Manages stepper motors and real-time odometry calculation.
 * 
 * This class runs a background FreeRTOS task to generate step pulses and 
 * updates the shared Position object based on differential drive kinematics.
 */
class MotorHandler {
public:
    MotorHandler();
    
    /**
     * @brief Testing constructor with injectable AccelStepper instances.
     * @param left Pointer to left motor (nullptr for default behavior)
     * @param right Pointer to right motor (nullptr for default behavior)
     */
    MotorHandler(AccelStepper* left, AccelStepper* right);
    
    ~MotorHandler();

    /**
     * @brief Starts the motor control task and initializes the odometry.
     * @param position Pointer to a thread-safe RAII Position object.
     * @param robotTrackWidth Distance between wheels in meters.
     */
    bool begin(RAII<Position>* position, float robotTrackWidth = 0.095, MotorConfig leftConfig = DEFAULT_LEFT_MOTOR, MotorConfig rightConfig = DEFAULT_RIGHT_MOTOR);

    /**
     * @brief Sets constant linear speeds for the motors (Open Loop).
     * @param leftSpeed Speed in m/s.
     * @param rightSpeed Speed in m/s.
     */
    void setSpeeds(float leftSpeed, float rightSpeed);

    /**
     * @brief Commands both motors to move a relative distance in steps.
     * @param distance Distance in steps.
     */
    void go(int distance);

    /**
     * @brief Performs an in-place turn by a specific angle.
     * @param angle_deg Degrees (positive for Counter-Clockwise).
     */
    void turn(float angle_deg);

    /**
     * @brief Triggers a smooth deceleration ramp to a full stop.
     */
    void stop();

    /**
     * @brief Checks if the robot is currently executing a motion.
     * @return true if the state is not IDLE.
     */
    bool isRunning();

    /**
     * @brief Internal FreeRTOS loop. Should not be called directly.
     */
    void run();

private:
    void calculateMotorConstants(const MotorConfig& config, double& metersPerStep, float& stepDistance);

    double metersPerStepL;
    double metersPerStepR;
    float stepDistanceL;
    float stepDistanceR;

    float trackWidth;

    RAII<Position>* position; // Pointer to shared memory protected by a mutex

    SemaphoreHandle_t motorMutex; // Protects access to MotorState and AccelStepper
    MotorState state;

    AccelStepper* pMotorL;
    AccelStepper* pMotorR;
};

#endif