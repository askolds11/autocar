#ifndef DRIVER
#define DRIVER

#include "Motor.hpp"

class Driver
{
private:
    int in1Pin;
    int in2Pin;
    int in3Pin;
    int in4Pin;
    int sleepPin;
    int faultPin;

public:
    Motor motors[2];
    Driver(int in1Pin, int in2Pin, int in3Pin, int in4Pin, int sleepPin, int faultPin);
    void initPins();
    void enable();
    void disable();
};

#endif