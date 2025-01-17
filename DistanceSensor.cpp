#include "DistanceSensor.hpp"
#include <stdio.h>
#include "vl6180x/inc/vl6180x_api.h"

// https://www.st.com/resource/en/application_note/an4545-vl6180x-basic-ranging-application-note-stmicroelectronics.pdf

DistanceSensor::DistanceSensor(int sdaPin, int sclPin, i2c_inst *i2cInst, int sensorNum, I2CMultiplexer *multiplexer)
    : sdaPin(sdaPin), sclPin(sclPin), i2cInst(i2cInst), sensorNum(sensorNum), i2cMultiplexer(multiplexer)
{
    printf("Initting sensor %d: ", sensorNum);
    multiplexer->setDevice(sensorNum);
    myDev = {.i2cInst = i2cInst, .i2cAddress = i2cAddress};
    device = &myDev;

    printf("Writing stuff... ");

    printf("Wrote. Waiting for boot... ");
    VL6180x_WaitDeviceBooted(device);
    printf("Booted.  Initting data... ");
    VL6180x_InitData(device);
    printf("Initted data. Preparing... ");
    VL6180x_Prepare(device);
    printf("Prepared\n");
}

/**
 * Implement Offset calibration as described in VL6180x Datasheet
 *
 * Device must be initialized
 *
 * @note device scaling wrap filter and xtalk setting are scraped !
 *  It is safer to reset and re init device when done
 * @warning  follow strictly procedure described in the device manual
 * @param myDev  The device
 * @return The offset value (signed interger)
 */
int DistanceSensor::sample_OffsetRunCalibration()
{
    i2cMultiplexer->setDevice(sensorNum);
    VL6180x_RangeData_t Range[N_MEASURE_AVG];
    int32_t RangeSum;
    int status;
    int i;
    int offset;
    int RealTargetDistance;

    /* Real target distance is 50 mm in proximity ranging configuration (scaling x1) or 100 mm in extended-range configuration */
    RealTargetDistance = (VL6180x_UpscaleGetScaling(device) == 1) ? 50 : 100;

    /* Turn off wrap-around filter (to avoid first invalid distances and decrease number of I2C accesses at maximum) */
    VL6180x_FilterSetState(device, 0);

    /* Clear all interrupts */
    status = VL6180x_ClearAllInterrupt(device);
    if (status)
    {
        printf("VL6180x_ClearAllInterrupt  fail");
    }

    /* Ask user to place a white target at know RealTargetDistance */
    printf("Calibrating %d: place white target at %dmm\n", sensorNum, RealTargetDistance);

    /* Program a null offset */
    VL6180x_SetOffsetCalibrationData(device, 0);

    /* Perform several ranging measurement */
    for (i = 0; i < N_MEASURE_AVG; i++)
    {
        status = VL6180x_RangePollMeasurement(device, &Range[i]);
        if (status)
        {
            printf("VL6180x_RangePollMeasurement  fail");
        }
        if (Range[i].errorStatus != 0)
        {
            printf("No target detect");
        }
    }

    /* Calculate ranging average (sum) */
    RangeSum = 0;
    for (i = 0; i < N_MEASURE_AVG; i++)
    {
        RangeSum += Range[i].range_mm;
    }

    /* Calculate part-to-part offset */
    offset = RealTargetDistance - (RangeSum / N_MEASURE_AVG);
    printf("offset for %d: %dmm\n", sensorNum, offset);
    return offset;
}

void DistanceSensor::setOffsetCalibrationData(int offset)
{
    i2cMultiplexer->setDevice(sensorNum);
    printf("Setting sensor %d offset to %dmm", sensorNum, offset);
    VL6180x_SetOffsetCalibrationData(device, offset);
}

void DistanceSensor::enableFilter()
{
    i2cMultiplexer->setDevice(sensorNum);
    VL6180x_FilterSetState(device, 1);
}

void DistanceSensor::rangeSync()
{
    i2cMultiplexer->setDevice(sensorNum);
    VL6180x_RangePollMeasurement(device, &Range);
}

void DistanceSensor::rangeAsyncStart()
{
    i2cMultiplexer->setDevice(sensorNum);
    VL6180x_RangeStartSingleShot(device);
}

bool DistanceSensor::rangeAsyncReady()
{
    i2cMultiplexer->setDevice(sensorNum);
    int status = VL6180x_RangeGetMeasurementIfReady(device, &Range);
    if (status != 0)
    {
        printf("Critical error for sensor num %d", sensorNum);
    }
    if (Range.errorStatus == RangeError_u::DataNotReady)
    {
        return false;
    }
    return true;
}

void DistanceSensor::rangeAsyncAwait()
{
    i2cMultiplexer->setDevice(sensorNum);
    while (!rangeAsyncReady())
        ; // wait until it's ready
}

RangeError DistanceSensor::getError()
{
    if (Range.errorStatus == RangeError_u::NoError)
    {
        return RangeError::OK;
    }
    else if (
        Range.errorStatus == RangeError_u::Early_Convergence_Estimate ||
        Range.errorStatus == RangeError_u::Max_Convergence ||
        Range.errorStatus == RangeError_u::Raw_Ranging_Algo_Overflow ||
        Range.errorStatus == RangeError_u::RangingFiltered ||
        Range.errorStatus == RangeError_u::Ranging_Algo_Overflow)
    {
        return RangeError::TOO_FAR;
    }
    else if (Range.errorStatus == RangeError_u::Raw_Ranging_Algo_Underflow || Range.errorStatus == RangeError_u::Ranging_Algo_Underflow)
    {
        return RangeError::TOO_CLOSE;
    }
    return RangeError::UNKNOWN;
}

int DistanceSensor::getDistance()
{
    if (Range.errorStatus != 0)
    {
        return -1;
    }
    return Range.range_mm;
}

void DistanceSensor::printRange(const char *sensorName)
{
    if (Range.errorStatus == RangeError_u::NoError)
    {
        printf("%s: %-5d \t | ", sensorName, Range.range_mm);
    }
    else
    {
        printf("%s: E%-5d \t | ", sensorName, Range.errorStatus);
    }
}
