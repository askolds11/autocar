#include "Motor.hpp"
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include "Globals.hpp"
#include <hardware/gpio.h>

Motor::Motor(int in1Pin, int in2Pin) : in1Pin(in1Pin), in2Pin(in2Pin)
{
    this->initPins();
}

void Motor::initPins()
{
    this->setPwmPin(this->in1Pin, MOTOR_PWM_FREQ);
    this->setPwmPin(this->in2Pin, MOTOR_PWM_FREQ);
}

void Motor::setSpeedAndDirection(float speed, bool forward)
{
    this->forwardDir = forward;
    this->speed = speed;

    int pwmPin = forward ? this->in1Pin : this->in2Pin;
    int dirPin = forward ? this->in2Pin : this->in1Pin;

    pwm_set_gpio_level(pwmPin, this->GetSpeed(speed));
    pwm_set_gpio_level(dirPin, 0);
}

void Motor::setSpeed(float speed)
{
    this->speed = speed;

    int pwmPin = this->forwardDir ? this->in1Pin : this->in2Pin;
    int dirPin = this->forwardDir ? this->in2Pin : this->in1Pin;

    pwm_set_gpio_level(pwmPin, this->GetSpeed(speed));
    pwm_set_gpio_level(dirPin, 0);
}

void Motor::setDirection(bool forward)
{
    // If direction is the same, do nothing
    if (this->forwardDir == forward)
    {
        return;
    }

    this->forwardDir = forward;

    int pwmPin = forward ? this->in1Pin : this->in2Pin;
    int dirPin = forward ? this->in2Pin : this->in1Pin;

    pwm_set_gpio_level(pwmPin, this->GetSpeed(speed));
    pwm_set_gpio_level(dirPin, 0);
}

int Motor::GetSpeed(float dutyCycle)
{
    return MAX_MOTOR_SPEED * dutyCycle / 100;
}

/// @brief Sets up pin for pwm output
/// @param pin Pin number
/// @param freq Frequency in Hz
void Motor::setPwmPin(uint pin, uint freq)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();

    float div = (float)clock_get_hz(clk_sys) / (freq * MAX_MOTOR_SPEED);
    pwm_config_set_clkdiv(&config, div);
    pwm_config_set_wrap(&config, MAX_MOTOR_SPEED);
    pwm_init(slice_num, &config, true); // start the pwm running according to the config
}