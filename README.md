# ECN Nantrobot - Eurobot Firmware (ESP32)

This repository contains the firmware developed by the **ECN Nantrobot** club for the **Eurobot** competition. 

The firmware provides a thread-safe Hardware Abstraction Layer (HAL) for an ESP32, designed to bridge the gap between low-level actuators and high-level strategy logic.

## 🏗️ Architecture Overview

The system is designed to be modular, separating the "Body" (Hardware) from the "Brain" (Strategy):

1.  **Hardware Abstraction Layer (HAL):**
    *   **`Robot` Class (`robot.h`):** The central hub. It bundles motor control, servo management, and odometry into one interface.
    *   **`MotorHandler`:** Manages stepper motors using `AccelStepper`. It runs in a dedicated FreeRTOS task to ensure precise stepping and real-time odometry calculation.
    *   **`ServoHandler`:** Manages Feetech STS-series serial bus servos, supporting torque control and telemetry feedback.
    *   **`RAII<Position>`:** A thread-safe wrapper that protects the robot's coordinates from "torn reads" when accessed by multiple tasks (e.g., motor task vs. serial task).

2.  **Strategy Layer (Application Logic):**
    *   The `Strategy` base class defines the interface.
    *   **Note:** The provided strategies (`RosStrategy`, `AutonomousStrategy`, `TestStrategy`) are **template examples**. They are meant to be used as starting points for building actual match missions or external control interfaces.

## ⚙️ Configuration

Settings are managed in `include/config.h`.

### 1. Selecting a Strategy
To switch between operational modes, uncomment **one** of the following lines:
```cpp
#define STRATEGY_ROS          // External control (Raspberry Pi/ROS)
// #define STRATEGY_AUTONOMOUS   // Standalone state machine
// #define STRATEGY_TEST         // Hardware diagnostic
```

### 2. Physical Tuning
Adjust parameters like `TRACK_WIDTH`, and motor pinouts directly in `config.h` to match your specific robot build.

## 🚀 Build & Upload

This project uses **PlatformIO**.

1.  **Build:** `pio run`
2.  **Upload:** `pio run -t upload`
3.  **Monitor:** `pio device monitor`
or just 
```bash
pio run -t upload && pio device monitor
```

## 🧪 Testing

Unit tests run on desktop (no hardware required):

```bash
pio test
```

Tests cover:
- **Servo/Motor math**: Angle conversions, odometry calculations
- **State machines**: IDLE → VELOCITY/POSITION transitions
- **HAL integration**: Robot initialization, component access

See `test/README` for details.

## 🧩 Directory Structure

```
├── include/
│   ├── config.h            # Physical constants & Strategy selection
│   ├── motorHandler.h      # Stepper driver & Odometry logic
│   ├── servoHandler.h      # Serial Bus Servo driver
│   ├── robot.h             # Main HAL interface
│   ├── utils.h             # Thread-safe (RAII) utility
│   └── strategies/         # Example Strategy implementations
│       ├── Strategy.h      # Abstract Base Class
│       ├── RosStrategy.h   # Serial Command example
│       └── ...
├── src/
│   ├── main.cpp            # Entry point & Strategy dispatcher
│   ├── motorHandler.cpp    # Motor background task implementation
│   └── servoHandler.cpp    # Servo communication implementation
└── platformio.ini          # PlatformIO dependencies & settings
```
