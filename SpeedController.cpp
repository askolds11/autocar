#include "SpeedController.hpp"
#include "Globals.hpp"
#include <stdio.h>

SpeedController::SpeedController(Driver *left, Driver *right) : left(left), right(right)
{
}

void SpeedController::spinLeft()
{
    printf("TURN LEFT\n");
    left->motors[0].setSpeed(-TURN_SPEED);
    left->motors[1].setSpeed(-TURN_SPEED);
    right->motors[0].setSpeed(TURN_SPEED);
    right->motors[1].setSpeed(TURN_SPEED);
}

void SpeedController::spinRight()
{
    printf("TURN RIGHT\n");
    left->motors[0].setSpeed(TURN_SPEED);
    left->motors[1].setSpeed(TURN_SPEED);
    right->motors[0].setSpeed(-TURN_SPEED);
    right->motors[1].setSpeed(-TURN_SPEED);
}

void SpeedController::goForwards()
{
    // TODO: Why minus
    printf("GO FORWARDS\n");
    left->motors[0].setSpeed(-CRUISE_SPEED);
    left->motors[1].setSpeed(-CRUISE_SPEED);
    right->motors[0].setSpeed(-CRUISE_SPEED);
    right->motors[1].setSpeed(-CRUISE_SPEED);
}

void SpeedController::stop()
{
    left->motors[0].setSpeed(0);
    left->motors[1].setSpeed(0);
    right->motors[0].setSpeed(0);
    right->motors[1].setSpeed(0);
}
