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

void Motor::invertDirection(bool value)
{
    invert = value;
}


void Motor::setSpeed(float speed)
{
    float actualSpeed = speed >= 0 ? speed : -speed;
    Direction direction = speed >= 0 ? Direction::FORWARDS : Direction::BACKWARDS;

    // if invert, change direction
    if (invert) {
        direction = direction == Direction::FORWARDS ? Direction::BACKWARDS : Direction::FORWARDS;
    }

    // If same, don't do anything
    if (this->speed == actualSpeed && this->direction == direction) {
        return;
    }

    // Update stored values
    this->speed = actualSpeed;
    this->direction = direction;

    // Choose which pin is pwm or dir based on direction
    int pwmPin = direction == Direction::FORWARDS ? this->in2Pin : this->in1Pin;
    int dirPin = direction == Direction::FORWARDS ? this->in1Pin : this->in2Pin;

    // Set pin values
    pwm_set_gpio_level(pwmPin, this->GetSpeed(speed));
    pwm_set_gpio_level(dirPin, 1); // slow decay
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
