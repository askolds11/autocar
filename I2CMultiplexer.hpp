#ifndef I2C_MULTIPLEXER
#define I2C_MULTIPLEXER

#include "hardware/i2c.h"

class I2CMultiplexer
{

private:
    int a0Pin;
    int a1Pin;
    int a2Pin;
    int sdaPin;
    int sclPin;
    i2c_inst *i2cInst;
    uint8_t i2cAddress = 0x70;

    /// @brief Initializes pins for the multiplexer
    void initPins();

public:
    /// @brief
    /// @param a0Pin
    /// @param a1Pin
    /// @param a2Pin
    /// @param sdaPin
    /// @param sclPin
    /// @param i2cInst
    I2CMultiplexer(int a0Pin, int a1Pin, int a2Pin, int sdaPin, int sclPin, i2c_inst *i2cInst);
    /// @brief Sets the active I2C device
    /// @param number 0-7
    void setDevice(int number);
};

#endif