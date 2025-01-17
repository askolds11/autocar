#ifndef MOTOR
#define MOTOR

#include "pico/types.h"

enum Direction {
    FORWARDS,
    BACKWARDS
};

class Motor
{
private:
    Direction direction;
    bool invert = false;
    bool forwardDir = false;
    float speed = 0;
    int in1Pin;
    int in2Pin;

    /// @brief Sets up pwm pin with given frequency
    /// @param pin Pin
    /// @param freq Frequency in Hz
    void setPwmPin(uint pin, uint freq);

public:
    Motor(int in1Pin, int in2Pin);
    /// @brief
    void initPins();

    /// @brief Sets motor's direction
    /// @param value true/false
    void invertDirection(bool value);

    /// @brief Sets motor's speed and direction
    /// @param speed Speed (-100-100), negative for backwards
    void setSpeed(float speed);

    /// @brief Gets level for pwm_set_gpio_level function
    /// @param dutyCycle Duty cycle 0-100
    /// @return
    static int GetSpeed(float dutyCycle);
};

#endif