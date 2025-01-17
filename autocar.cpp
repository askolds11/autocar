#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "Globals.hpp"
#include "Driver.hpp"
#include <stdio.h>
#include "I2CMultiplexer.hpp"
#include "DistanceSensor.hpp"
#include "SpeedController.hpp"

// Get distance of side.
// -1 if too close
// 999 if too far
// 1000 if unknown
int GetDistance(RangeError err1, RangeError err2, int dist1, int dist2)
{
    if (err1 == RangeError::OK && err2 == RangeError::OK)
    {
        return (dist1 + dist2) / 2;
    }
    else if (err1 == RangeError::OK)
    {
        // if too close, override
        if (err2 == RangeError::TOO_CLOSE)
        {
            return -1;
        }
        // otherwise return the ok value
        return dist1;
    }
    else if (err2 == RangeError::OK)
    {
        // if too close, override
        if (err1 == RangeError::TOO_CLOSE)
        {
            return -1;
        }
        // otherwise return the ok value
        return dist2;
    }
    else
    {
        // if one too close, return that
        if (err1 == RangeError::TOO_CLOSE || err2 == RangeError::TOO_CLOSE)
        {
            return -1;
        }
        else if (err1 == RangeError::TOO_FAR || RangeError::TOO_FAR)
        {
            return 999;
        }
        else
        {
            return 777;
        }
    }
}

int main()
{
    stdio_usb_init();

    printf("Starting\n");

    Driver driver1 = Driver(
        DRIVER_1_IN_1_PIN,
        DRIVER_1_IN_2_PIN,
        DRIVER_1_IN_3_PIN,
        DRIVER_1_IN_4_PIN,
        DRIVER_1_SLEEP_PIN,
        DRIVER_1_FAULT_PIN);

    Driver driver2 = Driver(
        DRIVER_2_IN_1_PIN,
        DRIVER_2_IN_2_PIN,
        DRIVER_2_IN_3_PIN,
        DRIVER_2_IN_4_PIN,
        DRIVER_2_SLEEP_PIN,
        DRIVER_2_FAULT_PIN);

    printf("Drivers done\n");

    SpeedController speedController = SpeedController(&driver1, &driver2);

    printf("Speed controller done\n");

    I2CMultiplexer multiplexer = I2CMultiplexer(
        I2C_A0,
        I2C_A1,
        I2C_A2,
        I2C_SDA,
        I2C_SCL,
        I2C_INST);

    printf("Multiplexer done\n");

    DistanceSensor sensorRB = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 0, &multiplexer);
    DistanceSensor sensorRF = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 1, &multiplexer);
    // DistanceSensor sensorAR = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 6, &multiplexer);
    DistanceSensor sensorF = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 3, &multiplexer);
    DistanceSensor sensorLF = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 4, &multiplexer);
    DistanceSensor sensorLB = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST, 5, &multiplexer);

    printf("After sensors\n");

    // TODO: https://www.st.com/resource/en/datasheet/vl6180.pdf 2.15.4 calibration?

    // printf("Getting offsets\n");
    // printf("Calibrate RB at 50mm\n");
    // sleep_ms(10000);
    // int offsetRB = sensorRB.sample_OffsetRunCalibration();
    // printf("Calibrate RF at 50mm\n");
    // sleep_ms(10000);
    // int offsetRF = sensorRF.sample_OffsetRunCalibration();
    // // printf("Calibrate AR at 50mm\n");
    // // sleep_ms(10000);
    // // int offsetAR = sensorAR.sample_OffsetRunCalibration();
    // printf("Calibrate AL at 50mm\n");
    // sleep_ms(10000);
    // int offsetAL = sensorF.sample_OffsetRunCalibration();
    // printf("Calibrate LF at 50mm\n");
    // sleep_ms(10000);
    // int offsetLF = sensorLF.sample_OffsetRunCalibration();
    // printf("Calibrate LB at 50mm\n");
    // sleep_ms(10000);
    // int offsetLB = sensorLB.sample_OffsetRunCalibration();

    // printf("Reenabling filters\n");
    // // TODO: Remove after hardcoding offsets
    // sensorRB.enableFilter();
    // sensorRF.enableFilter();
    // // sensorAR.enableFilter();
    // sensorF.enableFilter();
    // sensorLF.enableFilter();
    // sensorLB.enableFilter();

    // TODO: Hardcore offsets
    // 26; 32; 3; 35; 16
    // 27; 32; 3; 34; 16
    // 26; 32; 4; 35; 17
    printf("Settings offsets\n");
    sensorRB.setOffsetCalibrationData(26);
    sensorRF.setOffsetCalibrationData(32);
    // sensorAR.setOffsetCalibrationData(offsetAR);
    sensorF.setOffsetCalibrationData(3);
    sensorLF.setOffsetCalibrationData(35);
    sensorLB.setOffsetCalibrationData(16);

    int status;
    int status1;

    printf("Enabling drivers\n");
    driver1.enable();
    driver2.enable();

    printf("Inverting driver\n");
    driver1.motors[0].invertDirection(true);
    driver1.motors[1].invertDirection(true);

    RangeError errorRB;
    RangeError errorRF;
    // RangeError errorAR;
    RangeError errorF;
    RangeError errorLF;
    RangeError errorLB;

    int distRB;
    int distRF;
    // int distAR;
    int distF;
    int distLF;
    int distLB;

    RangeError errorPrefB;
    RangeError errorPrefF;
    int distPrefB;
    int distPrefF;
    int distPrefAvg;

    int minDistR;
    int minDistL;

    bool rightPreferred = true;
    int spunTimes = 0;

    printf("Starting\n");
    while (true)
    {
        // start measurment for all
        sensorRB.rangeAsyncStart();
        sensorRF.rangeAsyncStart();
        // sensorAR.rangeAsyncStart();
        sensorF.rangeAsyncStart();
        sensorLF.rangeAsyncStart();
        sensorLB.rangeAsyncStart();

        // await all
        sensorRB.rangeAsyncAwait();
        sensorRF.rangeAsyncAwait();
        // sensorAR.rangeAsyncAwait();
        sensorF.rangeAsyncAwait();
        sensorLF.rangeAsyncAwait();
        sensorLB.rangeAsyncAwait();

        // get errors
        errorRB = sensorRB.getError();
        errorRF = sensorRF.getError();
        // errorAR = sensorAR.getError();
        errorF = sensorF.getError();
        errorLF = sensorLF.getError();
        errorLB = sensorLB.getError();

        // get distances
        distRB = sensorRB.getDistance();
        distRF = sensorRF.getDistance();
        // distAR = sensorAR.getDistance();
        distF = sensorF.getDistance();
        distLF = sensorLF.getDistance();
        distLB = sensorLB.getDistance();

        // print sensors
        sensorRB.printRange("RB");
        sensorRF.printRange("RF");
        // sensorAR.printRange("AR");
        sensorF.printRange("AL");
        sensorLF.printRange("LF");
        sensorLB.printRange("LB");

        minDistR = GetDistance(errorRB, errorRF, distRB, distRF);
        minDistL = GetDistance(errorLB, errorLF, distLB, distLF);

        if (minDistR < minDistL)
        {
            errorPrefB = errorRB;
            errorPrefF = errorRF;
            distPrefB = distRB;
            distPrefF = distRF;
            distPrefAvg = minDistR;
            rightPreferred = true;
        }
        else if (minDistL < minDistR)
        {
            errorPrefB = errorLB;
            errorPrefF = errorLF;
            distPrefB = distLB;
            distPrefF = distLF;
            distPrefAvg = minDistL;
            rightPreferred = false;
        }
        else
        {
            if (rightPreferred)
            {
                errorPrefB = errorRB;
                errorPrefF = errorRF;
                distPrefB = distRB;
                distPrefF = distRF;
                distPrefAvg = minDistR;
            }
            else
            {
                errorPrefB = errorLB;
                errorPrefF = errorLF;
                distPrefB = distLB;
                distPrefF = distLF;
                distPrefAvg = minDistL;
            }
        }

        // Front is clear. Assume unknown is too far as well.
        if (errorF == RangeError::OK && distF >= MAX_FRONT || errorF == RangeError::TOO_FAR || errorF == RangeError::UNKNOWN)
        {
            // If both are OK, simple maths
            if (errorPrefB == RangeError::OK && errorPrefF == RangeError::OK)
            {
                // If too far, try to get closer
                if (distPrefAvg > PREFERRED_RANGE)
                {
                    // Turn until angle achieved
                    if (distPrefB - distPrefF < TURNING_DIFF)
                    {
                        if (rightPreferred)
                        {
                            speedController.spinRight();
                            spunTimes++;
                        }
                        else
                        {
                            speedController.spinLeft();
                            spunTimes++;
                        }
                    }
                    // Go forward after spinning
                    else
                        speedController.goForwards();
                }
                // If too close, try to get closer
                else if (distPrefAvg <= MIN_RANGE)
                {
                    // Turn until angle achieved
                    if (distPrefF - distPrefB < TURNING_DIFF)
                    {
                        if (rightPreferred)
                        {
                            speedController.spinLeft();
                            spunTimes++;
                        }
                        else
                        {
                            speedController.spinRight();
                            spunTimes++;
                        }
                    }
                    // Go forward after spinning
                    else
                        speedController.goForwards();
                }
                // If front further than back
                else if (distRF - distRB > MAX_DIFF)
                {
                    if (rightPreferred)
                        speedController.spinRight();
                    else
                        speedController.spinLeft();
                }
                // If back further than front
                else if (distRB - distRF > MAX_DIFF)
                {
                    if (rightPreferred)
                        speedController.spinLeft();
                    else
                        speedController.spinRight();
                }
                // If ok, continue forwards
                else
                    speedController.goForwards();
            }
            // Front is measurable
            else if (errorPrefF == RangeError::OK)
            {
                // If back is too close, spin to fix
                if (errorPrefB == RangeError::TOO_CLOSE)
                {
                    if (rightPreferred)
                        speedController.spinRight();
                    else
                        speedController.spinLeft();
                }
                // If back is too far, spin to fix
                else if (errorPrefB == RangeError::TOO_FAR)
                {
                    if (rightPreferred)
                        speedController.spinLeft();
                    else
                        speedController.spinRight();
                }
                // Unknown error, yolo
                else
                {
                    speedController.goForwards();
                }
            }
            // Back is measurable
            else if (errorPrefB == RangeError::OK)
            {
                // If front is too close, spin to fix
                if (errorPrefF == RangeError::TOO_CLOSE)
                {
                    if (rightPreferred)
                        speedController.spinLeft();
                    else
                        speedController.spinRight();
                }
                // If front is too far, spin to fix
                else if (errorPrefF == RangeError::TOO_FAR)
                {
                    if (rightPreferred)
                        speedController.spinRight();
                    else
                        speedController.spinLeft();
                }
                // Unknown error, yolo
                else
                {
                    speedController.goForwards();
                }
            }
            // Both errors
            else
            {
                // If too close, spin front away from the wall, then go forwards
                if (errorPrefB == RangeError::TOO_CLOSE && errorPrefF == RangeError::TOO_CLOSE)
                {
                    // Turn for spin cycles
                    if (spunTimes <= SPIN_CYCLES)
                    {
                        if (rightPreferred)
                        {
                            speedController.spinLeft();
                            spunTimes++;
                        }
                        else
                        {
                            speedController.spinRight();
                            spunTimes++;
                        }
                    }
                    // Go forward after spinning
                    else
                        speedController.goForwards();
                }
                // If too far, spin front towards the wall, then go forwards
                else if (errorPrefB == RangeError::TOO_FAR && errorPrefF == RangeError::TOO_FAR)
                {
                    // Turn for spin cycles
                    if (spunTimes <= SPIN_CYCLES)
                    {
                        if (rightPreferred)
                        {
                            speedController.spinRight();
                            spunTimes++;
                        }
                        else
                        {
                            speedController.spinLeft();
                            spunTimes++;
                        }
                    }
                    // Go forward after spinning
                    else
                        speedController.goForwards();
                }
                // If back too close, spin front towards wall
                else if (errorPrefB == RangeError::TOO_CLOSE)
                {
                    if (rightPreferred)
                    {
                        speedController.spinRight();
                    }
                    else
                    {
                        speedController.spinLeft();
                    }
                }
                // If front too close, spin front away from wall
                else if (errorPrefF == RangeError::TOO_CLOSE)
                {
                    if (rightPreferred)
                    {
                        speedController.spinLeft();
                    }
                    else
                    {
                        speedController.spinRight();
                    }
                }
                // Back/Front is too far, other is unknown (too far?)
                else if (errorPrefB == RangeError::TOO_FAR)
                {
                    // Turn for spin cycles
                    if (spunTimes <= SPIN_CYCLES)
                    {
                        if (rightPreferred)
                        {
                            speedController.spinRight();
                            spunTimes++;
                        }
                        else
                        {
                            speedController.spinLeft();
                            spunTimes++;
                        }
                    }
                    // Go forward after spinning
                    else
                        speedController.goForwards();
                }
                // YOLO
                else
                {
                    speedController.goForwards();
                }
            }
        }
        // Front is not clear. Turn immediately
        else
        {
            // Reset spunTimes
            spunTimes = 0;
            if (rightPreferred)
            {
                // front obstructed, things on right, turn left
                if (distPrefAvg < 777)
                    speedController.spinLeft();
                // front obstructed, nothing on right, turn right
                else
                    speedController.spinRight();
            }
            else
            {
                // front obstructed, things on left, turn right
                if (minDistL < 777)
                    speedController.spinRight();
                // front obstructed, nothing on left, turn left
                else
                    speedController.spinLeft();
            }
        }

        // If distance fixed, reset spunTimes
        if (distPrefAvg <= PREFERRED_RANGE && distPrefAvg != -1)
        {
            spunTimes = 0;
        }
    }
}
