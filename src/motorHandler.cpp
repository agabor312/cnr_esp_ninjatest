#include <math.h>
#include <AccelStepper.h>
#include "motorHandler.h"

// Static trampoline for FreeRTOS task
static void motorRunWrapper(void* pvParameters) {
    MotorHandler* handler = static_cast<MotorHandler*>(pvParameters);
    handler->run(); // Calls the class method
}

MotorHandler::MotorHandler() {
    position = nullptr;
    motorMutex = nullptr;
    state = IDLE;
    metersPerStepL = 0;
    metersPerStepR = 0;
    stepDistanceL = 0;
    stepDistanceR = 0;
    trackWidth = 0;
    pMotorL = nullptr;
    pMotorR = nullptr;
}

MotorHandler::~MotorHandler() {
    if (pMotorL) delete pMotorL;
    if (pMotorR) delete pMotorR;
    if (motorMutex) vSemaphoreDelete(motorMutex);
}

void MotorHandler::calculateMotorConstants(const MotorConfig& config, double& metersPerStep, float& stepDistance) {
    double oneStepAngle = 2.0 * M_PI / config.steps_per_revolution; // [rad]
    metersPerStep = oneStepAngle * config.wheelDiameter / 2.0; // [m]
    stepDistance = (M_PI * config.wheelDiameter) / config.steps_per_revolution; // [m]
}

bool MotorHandler::begin(RAII<Position>* pos, float robotTrackWidth, MotorConfig leftConfig, MotorConfig rightConfig) {
    this->position = pos;
    this->trackWidth = robotTrackWidth;

    // Allocate Motors
    pMotorL = new AccelStepper(leftConfig.motorInterfaceType, leftConfig.step_pin, leftConfig.dir_pin);
    pMotorR = new AccelStepper(rightConfig.motorInterfaceType, rightConfig.step_pin, rightConfig.dir_pin);

    // Calculate Constants
    calculateMotorConstants(leftConfig, metersPerStepL, stepDistanceL);
    calculateMotorConstants(rightConfig, metersPerStepR, stepDistanceR);

    // Initialize AccelStepper instances
    pMotorL->setMaxSpeed(leftConfig.max_speed);
    pMotorL->setAcceleration(leftConfig.acceleration);
    pMotorL->setPinsInverted(leftConfig.direction_invert, leftConfig.step_invert, leftConfig.enable_invert);

    pMotorR->setMaxSpeed(rightConfig.max_speed);
    pMotorR->setAcceleration(rightConfig.acceleration);
    pMotorR->setPinsInverted(rightConfig.direction_invert, rightConfig.step_invert, rightConfig.enable_invert);

    pMotorL->setCurrentPosition(0);
    pMotorR->setCurrentPosition(0);

    // Set initial state
    state = IDLE;

    motorMutex = xSemaphoreCreateMutex();
    if (motorMutex == NULL) {
        return false;
    }

    // Create the background task
    xTaskCreatePinnedToCore(
        motorRunWrapper, // Task function wrapper
        "motorRun",         // Name
        4096,                // Stack size
        this,                // Pass 'this' as parameter
        1,                   // Priority
        NULL,                // Task handle
        1                    // CPU core (1)
    );

    return true;
}

void MotorHandler::setSpeeds(float leftSpeed, float rightSpeed) {
    if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE) {
        // Convert speed from [m/s] to [steps/s] using per-motor constants
        float leftStepSpeed = leftSpeed / metersPerStepL;
        float rightStepSpeed = rightSpeed / metersPerStepR;

        pMotorL->setSpeed(leftStepSpeed);
        pMotorR->setSpeed(rightStepSpeed);

        state = VELOCITY_MODE;
        xSemaphoreGive(motorMutex);
    }
}

void MotorHandler::go(int distance) {
    if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE) {
        pMotorL->move(distance);
        pMotorR->move(distance);
        state = POSITION_MODE;
        xSemaphoreGive(motorMutex);
    }
}

void MotorHandler::turn(float angle_deg) {
    if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE) {
        // Convert angle to radians
        float angle_rad = angle_deg * (M_PI / 180.0);

        // Calculate arc length for each wheel to turn the robot in place
        // For a differential drive robot turning in place, the wheels move along a circle with diameter = trackWidth
        // Distance each wheel travels = (angle_rad * trackWidth) / 2
        float arc_length = (angle_rad * trackWidth) / 2.0;

        // Convert arc length to steps
        // For CCW turn (+angle): Right wheel forward (+), Left wheel backward (-)
        long stepsL = -static_cast<long>(arc_length / metersPerStepL);
        long stepsR = static_cast<long>(arc_length / metersPerStepR);

        pMotorL->move(stepsL);
        pMotorR->move(stepsR);

        state = POSITION_MODE;
        xSemaphoreGive(motorMutex);
    }
}

void MotorHandler::stop() {
    if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE) {
        pMotorL->stop();
        pMotorR->stop();
        // Allow the run loop to execute the deceleration steps
        state = POSITION_MODE;
        xSemaphoreGive(motorMutex);
    }
}

bool MotorHandler::isRunning() {
    return state != IDLE;
}

void MotorHandler::run() {
    while (1) {
        long deltaL = 0;
        long deltaR = 0;

        if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE) {

            long posL_before = pMotorL->currentPosition();
            long posR_before = pMotorR->currentPosition();

            switch (state) {
                case VELOCITY_MODE:
                    pMotorL->runSpeed();
                    pMotorR->runSpeed();
                    break;

                case POSITION_MODE:
                    {
                        bool isRunningL = pMotorL->run();
                        bool isRunningR = pMotorR->run();

                        // If reached target, switch to IDLE
                        if (!isRunningL && !isRunningR) {
                            state = IDLE;
                        }
                    }
                    break;

                case IDLE:
                default:
                    // Do nothing
                    break;
            }

            long posL_after = pMotorL->currentPosition();
            long posR_after = pMotorR->currentPosition();

            deltaL = posL_after - posL_before;
            deltaR = posR_after - posR_before;

            xSemaphoreGive(motorMutex);
        }

        // --- Odometry Update ---
        if ((deltaL != 0 || deltaR != 0) && position != nullptr) {
            // Calculate distance traveled based on signed delta steps
            float dL = deltaL * metersPerStepL;
            float dR = deltaR * metersPerStepR;

            // Differential drive equations
            float d_center = (dL + dR) / 2.0;
            float dTheta = (dR - dL) / trackWidth;

            // Lock the position object for update
            auto pos = position->lock();
            float thetaMid = pos->theta + dTheta / 2.0;

            pos->x += d_center * cos(thetaMid);
            pos->y += d_center * sin(thetaMid);
            pos->theta += dTheta;
        }

        // --- Yielding Logic ---
        if (state == IDLE) {
            vTaskDelay(1);
        }
    }
}
