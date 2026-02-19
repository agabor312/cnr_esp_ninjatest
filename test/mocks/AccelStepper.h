#ifndef AccelStepper_h
#define AccelStepper_h

#include "Arduino.h"

class AccelStepper {
public:
    // Constructor
    AccelStepper(uint8_t interface = 1, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true) {}

    // Configuration
    void setMaxSpeed(float speed) { _maxSpeed = speed; }
    void setAcceleration(float acceleration) { _acceleration = acceleration; }
    void setSpeed(float speed) { _speed = speed; }
    void setPinsInverted(bool directionInvert, bool stepInvert, bool enableInvert) {}
    void setCurrentPosition(long position) { _currentPos = position; }

    // Movement
    void move(long relative) { _targetPos = _currentPos + relative; _isRunning = true; }
    void moveTo(long absolute) { _targetPos = absolute; _isRunning = true; }
    void stop() { _targetPos = _currentPos; _isRunning = false; }

    // Execution
    boolean run() {
        if (_currentPos < _targetPos) {
            _currentPos++;
            return true;
        } else if (_currentPos > _targetPos) {
            _currentPos--;
            return true;
        }
        _isRunning = false;
        return false;
    }

    boolean runSpeed() {
        if (_speed > 0) _currentPos++;
        else if (_speed < 0) _currentPos--;
        // runSpeed returns true if a step was made? Yes, usually.
        // For testing, let's say it always steps if speed != 0.
        return _speed != 0;
    }

    // Status
    long distanceToGo() { return _targetPos - _currentPos; }
    long targetPosition() { return _targetPos; }
    long currentPosition() { return _currentPos; }
    boolean isRunning() { return _isRunning; }
    float speed() { return _speed; }

private:
    long _currentPos = 0;
    long _targetPos = 0;
    float _speed = 0;
    float _maxSpeed = 0;
    float _acceleration = 0;
    bool _isRunning = false;
};

#endif
