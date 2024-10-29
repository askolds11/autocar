#include "I2CMultiplexer.hpp"
#include <hardware/gpio.h>
#include "hardware/i2c.h"

I2CMultiplexer::I2CMultiplexer(int a0Pin, int a1Pin, int a2Pin, int sdaPin, int sclPin, i2c_inst *i2cInst)
    : a0Pin(a0Pin), a1Pin(a1Pin), a2Pin(a2Pin), sdaPin(sdaPin), sclPin(sclPin), i2cInst(i2cInst)
{
    this->initPins();
    // TODO: Make hardcoded address depend on configuration of a0, a1, a2
}

void I2CMultiplexer::initPins()
{
    // A pins
    gpio_init(this->a0Pin);
    gpio_set_dir(this->a0Pin, GPIO_OUT);
    gpio_init(this->a1Pin);
    gpio_set_dir(this->a1Pin, GPIO_OUT);
    gpio_init(this->a2Pin);
    gpio_set_dir(this->a2Pin, GPIO_OUT);
    gpio_put(this->a0Pin, 0);
    gpio_put(this->a1Pin, 0);
    gpio_put(this->a2Pin, 0);

    // I2C pins
    gpio_set_dir(this->sdaPin, GPIO_FUNC_I2C);
    gpio_set_dir(this->sclPin, GPIO_FUNC_I2C);
    gpio_pull_up(this->sdaPin);
    gpio_pull_up(this->sclPin);

    // I2C init
    i2c_init(i2cInst, 400000);
}

void I2CMultiplexer::setDevice(int number)
{
    uint8_t recData;
    // i2c_read_blocking(i2cInst, i2cAddress, &recData, 1)
    uint8_t buf[1];
    buf[0] = 1 << number;
    i2c_write_blocking(i2cInst, i2cAddress, buf, 1, false);
}
