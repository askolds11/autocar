#ifndef SPEEDCONTROLLER
#define SPEEDCONTROLLER

#include "Driver.hpp"

class SpeedController
{
private:
    Driver* left;
    Driver* right;

public:
    SpeedController(Driver* left, Driver* right);
    void spinLeft();
    void spinRight();
    void goForwards();
    void stop();
};

#endif