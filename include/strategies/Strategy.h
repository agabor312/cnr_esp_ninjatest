#ifndef STRATEGY_H
#define STRATEGY_H

#include <Arduino.h>
#include "robot.h"

/**
 * @brief Abstract Base Class defining the contract for all Robot Strategies.
 * 
 * Each mission or operational mode (ROS, Autonomous, Test) must inherit from 
 * this class and implement setup() and loop().
 */
class Strategy {
protected:
    Robot& robot; // Reference to the shared hardware instance

public:
    // Constructor links the brain (Strategy) to the body (Robot)
    Strategy(Robot& r) : robot(r) {}
    
    virtual ~Strategy() {} // Virtual destructor ensures children are cleaned up correctly
    
    /**
     * @brief Called once by the main firmware setup()
     */
    virtual void setup() = 0;

    /**
     * @brief Called repeatedly by the main firmware loop()
     */
    virtual void loop() = 0;
};

#endif // STRATEGY_H
