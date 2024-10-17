#ifndef MOTOR
#define MOTOR

#include "pico/types.h"

class Motor
{
private:
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

    /// @brief Sets motor's speed and direction
    /// @param speed Speed (0-100)
    /// @param forward Is forward direction
    void setSpeedAndDirection(float speed, bool forward);

    /// @brief Sets motor's speed
    /// @param speed Speed (0-100)
    void setSpeed(float speed);

    /// @brief Sets motor's direction
    /// @param forward Is forward direction
    void setDirection(bool forward);

    /// @brief Gets level for pwm_set_gpio_level function
    /// @param dutyCycle Duty cycle 0-100
    /// @return
    static int GetSpeed(float dutyCycle);
};

#endif