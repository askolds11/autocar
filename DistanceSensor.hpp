#ifndef DISTANCE_SENSOR
#define DISTANCE_SENSOR

#include "hardware/i2c.h"
#include "I2CMultiplexer.hpp"
#include "vl6180x/platform/template/vl6180x_platform.h"

#define N_MEASURE_AVG 10

enum RangeError {
    OK,
    TOO_FAR,
    TOO_CLOSE,
    UNKNOWN
};

class DistanceSensor
{
private:
    int sdaPin;
    int sclPin;
    i2c_inst *i2cInst;
    uint8_t i2cAddress = 0x29;
    int sensorNum;
    I2CMultiplexer* i2cMultiplexer;
    MyDev_t myDev;
    VL6180xDev_t device;
    VL6180x_RangeData_t Range;

public:
    /// @brief
    /// @param sdaPin
    /// @param sclPin
    /// @param i2cInst
    DistanceSensor(int sdaPin, int sclPin, i2c_inst *i2cInst, int sensorNum, I2CMultiplexer* multiplexer);
    // /// @brief Start a range measurement in single shot mode
    // void startRange();
    // /// @brief poll for new sample ready
    // void pollRange();
    // /// @brief Read range result (mm)
    // /// @return
    // uint8_t readRange();
    // /// @brief clear interrupts
    // void clearInterrupts();
    int sample_OffsetRunCalibration();
    void setOffsetCalibrationData(int offset);
    void enableFilter();
    void rangeSync();
    void rangeAsyncStart();
    bool rangeAsyncReady();
    void rangeAsyncAwait();
    RangeError getError();
    int getDistance();
    void printRange(const char* sensorName);
};

#endif