// This is a self-contained, powerful, header-only mock for the STS Servo Driver library.
// It defines a single STSServoDriver class that contains all the logic
// for simulating a bus of virtual servos for unit testing.

#ifndef MOCK_STSSERVO_DRIVER_H
#define MOCK_STSSERVO_DRIVER_H

#include <Arduino.h>
#include <map>

// Define the servo configuration constants locally to avoid circular dependency
// These match ST3215_CONFIG from servoHandler.h
constexpr byte STS_REG_TORQUE_SWITCH = 0x28;
constexpr byte STS_REG_TORQUE_LIMIT = 0x30;
constexpr byte STS_REG_VOLTAGE = 0x3E;

enum STSMode { POSITION = 0, VELOCITY = 1, STEP = 3 };

// A struct to hold the complete state of a single simulated servo.
struct VirtualServo {
    byte id = 0;
    byte mode = STSMode::POSITION;
    bool torque_enabled = true;
    int16_t torque_limit = 1000;

    int current_position = 0;
    int current_speed = 0;
    int current_temperature = 0;
    int16_t current_voltage_raw = 0;
    float current_current_amps = 0.0f;
    bool is_moving_status = false;

    int target_position = 0;
    int target_velocity = 0;
};

// This is the one and only mock class. It simulates the real library's behavior.
class STSServoDriver {
public:
    std::map<byte, VirtualServo> virtual_servos;

    // Helper to get or create a virtual servo
    VirtualServo& getServo(byte const& servoId) {
        if (virtual_servos.find(servoId) == virtual_servos.end()) {
            virtual_servos[servoId].id = servoId;
        }
        return virtual_servos.at(servoId);
    }

    // --- Mocked implementations of all library functions ---
    virtual ~STSServoDriver() {}

    virtual bool init(HardwareSerial *serialPort = nullptr, long const &baudRate = 1000000) { return true; }
    virtual bool ping(byte const &servoId) { return true; }

    virtual bool setMode(unsigned char const& servoId, STSMode const& mode) {
        getServo(servoId).mode = mode;
        return true;
    }

    virtual bool writeRegister(byte const &servoId, byte const &registerId, byte const &value, bool const &asynchronous = false) {
        if (registerId == STS_REG_TORQUE_SWITCH) {
            getServo(servoId).torque_enabled = (value == 1);
        }
        return true;
    }

    virtual bool writeTwoBytesRegister(byte const &servoId, byte const &registerId, int16_t const &value, bool const &asynchronous = false) {
        if (registerId == STS_REG_TORQUE_LIMIT) {
            getServo(servoId).torque_limit = value;
        }
        return true;
    }

    virtual int16_t readTwoBytesRegister(byte const &servoId, byte const &registerId) {
        if (registerId == STS_REG_VOLTAGE) {
            return getServo(servoId).current_voltage_raw;
        }
        return -1;
    }

    virtual int getCurrentPosition(byte const &servoId) { return getServo(servoId).current_position; }

    virtual bool setTargetPosition(byte const &servoId, int const &position, int const &speed = 4095, bool const &asynchronous = false) {
        VirtualServo& servo = getServo(servoId);
        servo.target_position = position;
        servo.target_velocity = speed;
        return true;
    }

    virtual int getCurrentSpeed(byte const &servoId) { return getServo(servoId).current_speed; }

    virtual bool setTargetVelocity(byte const &servoId, int const &velocity, bool const &asynchronous = false) {
        getServo(servoId).target_velocity = velocity;
        return true;
    }

    virtual int getCurrentTemperature(byte const &servoId) { return getServo(servoId).current_temperature; }
    virtual float getCurrentCurrent(byte const &servoId) { return getServo(servoId).current_current_amps; }
    virtual bool isMoving(byte const &servoId) { return getServo(servoId).is_moving_status; }
};

#endif // MOCK_STSSERVO_DRIVER_H
