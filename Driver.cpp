#include "Driver.hpp"
#include <hardware/gpio.h>

Driver::Driver(int in1Pin, int in2Pin, int in3Pin, int in4Pin, int sleepPin, int faultPin)
    : in1Pin(in1Pin), in2Pin(in2Pin), in3Pin(in3Pin), in4Pin(in4Pin), sleepPin(sleepPin), faultPin(faultPin),
      motors{Motor(in1Pin, in2Pin), Motor(in3Pin, in4Pin)}
{
    initPins();
}

void Driver::initPins()
{
    gpio_init(this->sleepPin);
    gpio_set_dir(this->sleepPin, GPIO_OUT);
    gpio_init(this->faultPin);
    gpio_set_dir(this->faultPin, GPIO_IN);
}
void Driver::enable()
{
    gpio_put(this->sleepPin, 1);
}

void Driver::disable()
{
    gpio_put(this->sleepPin, 0);
}