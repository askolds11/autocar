#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "Globals.hpp"
#include "Driver.hpp"
#include <stdio.h>
#include "I2CMultiplexer.hpp"
#include "DistanceSensor.hpp"
#include "vl6180x/inc/vl6180x_api.h"

#define N_MEASURE_AVG   10

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
int Sample_OffsetRunCalibration(VL6180xDev_t myDev)
{
    VL6180x_RangeData_t Range[N_MEASURE_AVG];
    int32_t   RangeSum;
    int status;
    int i;
    int offset;
    int RealTargetDistance;

    /* Real target distance is 50 mm in proximity ranging configuration (scaling x1) or 100 mm in extended-range configuration */
    RealTargetDistance = (VL6180x_UpscaleGetScaling(myDev)==1) ? 50 : 100;
    

    /* Turn off wrap-around filter (to avoid first invalid distances and decrease number of I2C accesses at maximum) */
    VL6180x_FilterSetState(myDev, 0);

    /* Clear all interrupts */
    status = VL6180x_ClearAllInterrupt(myDev);
    if( status ){
        printf("VL6180x_ClearAllInterrupt  fail");
    }

    /* Ask user to place a white target at know RealTargetDistance */
    printf("Calibrating : place white target at %dmm",RealTargetDistance);
    
    /* Program a null offset */
    VL6180x_SetOffsetCalibrationData(myDev, 0);
    
    /* Perform several ranging measurement */
    for( i=0; i<N_MEASURE_AVG; i++){
        status = VL6180x_RangePollMeasurement(myDev, &Range[i]);
        if( status ){
            printf("VL6180x_RangePollMeasurement  fail");
        }
        if( Range[i].errorStatus != 0 ){
            printf("No target detect");
        }
    }
    
    /* Calculate ranging average (sum) */
    RangeSum=0;
    for( i=0; i<N_MEASURE_AVG; i++){
        RangeSum+= Range[i].range_mm;
    }
    
    /* Calculate part-to-part offset */
    offset = RealTargetDistance - (RangeSum/N_MEASURE_AVG);
    printf("offset %d", offset);
    return offset;
}

int main()
{
    stdio_usb_init();

    sleep_ms(5000);

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

    I2CMultiplexer multiplexer = I2CMultiplexer(
        I2C_A0,
        I2C_A1,
        I2C_A2,
        I2C_SDA,
        I2C_SCL,
        I2C_INST);

    multiplexer.setDevice(0);

    DistanceSensor sensor = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST);

    sleep_us(2000);
    
    printf("After sensor");

    MyDev_t test = { .i2cInst = I2C_INST, .i2cAddress = 0x29};
    VL6180xDev_t myDev = &test;
    VL6180x_RangeData_t Range;

    // VL6180x_RangeData_t Range;
    // MyDev_Init(myDev);           // your code init device variable
    // MyDev_SetChipEnable(myDev);  // your code assert chip enable
    // MyDev_uSleep(1000);          // your code sleep at least 1 msec
    printf("Initting data");
    
    VL6180x_InitData(myDev);
    printf("Initted data");

    VL6180x_Prepare(myDev);
    printf("Prepared\n");

    int offset;
    int status;
    /* run calibration */
    printf("Calibrate 0 at 50mm!\n");
    sleep_ms(5000);
    offset = Sample_OffsetRunCalibration(myDev);

    /* when possible reset re-init device otherwise set back required filter */
    VL6180x_FilterSetState(myDev, 1);  // turn on wrap around filter again
    
    /* program offset */
    VL6180x_SetOffsetCalibrationData(myDev, offset);

    //
    //
    //
    //
    //

    multiplexer.setDevice(1);

    DistanceSensor sensor1 = DistanceSensor(I2C_SDA, I2C_SCL, I2C_INST);

    sleep_us(2000);
    
    printf("After sensor");

    MyDev_t test1 = { .i2cInst = I2C_INST, .i2cAddress = 0x29};
    VL6180xDev_t myDev1 = &test1;
    VL6180x_RangeData_t Range1;

    // VL6180x_RangeData_t Range;
    // MyDev_Init(myDev);           // your code init device variable
    // MyDev_SetChipEnable(myDev);  // your code assert chip enable
    // MyDev_uSleep(1000);          // your code sleep at least 1 msec
    printf("Initting data");
    
    VL6180x_InitData(myDev1);
    printf("Initted data");

    VL6180x_Prepare(myDev1);
    printf("Prepared\n");

    int offset1;
    int status1;
    /* run calibration */
    printf("Calibrate 1 at 50mm!\n");
    sleep_ms(5000);
    offset1 = Sample_OffsetRunCalibration(myDev1);

    /* when possible reset re-init device otherwise set back required filter */
    VL6180x_FilterSetState(myDev1, 1);  // turn on wrap around filter again
    
    /* program offset */
    VL6180x_SetOffsetCalibrationData(myDev1, offset1);
    

    // driver1.enable();

    // driver1.motors[0].setSpeed(50);
    // driver1.motors[1].setSpeed(50);

    // driver2.enable();

    // driver2.motors[0].setSpeed(50);
    // driver2.motors[1].setSpeed(50);

    while (true)
    {
        multiplexer.setDevice(0);
        VL6180x_RangePollMeasurement(myDev, &Range);
        if (Range.errorStatus == 0 ) {
            printf("ZERO: %-40d \t | ", Range.range_mm);
        }
            // MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
        else {
            printf("ZERO: %-40s \t | ", VL6180x_RangeGetStatusErrString(Range.errorStatus));
            // printf("errorstatus %u\n", Range.errorStatus);
        }
        multiplexer.setDevice(1);
        VL6180x_RangePollMeasurement(myDev1, &Range1);
        if (Range1.errorStatus == 0 ) {
            printf("ONE: %d\n", Range1.range_mm);
        }
            // MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
        else {
            printf("ONE: %s\n", VL6180x_RangeGetStatusErrString(Range1.errorStatus));
            // printf("errorstatus %u\n", Range.errorStatus);
        }
        // printf("Clearing interrupts...\n");
        // sensor.clearInterrupts();
        // printf("Starting...\n");
        // sensor.startRange();
        // // printf("Polling...\n");
        // sensor.pollRange();
        // // printf("Distance...\n");
        // uint8_t distance = sensor.readRange();
        // printf("%u\n", distance);
        // // printf("Hello World From Pi Pico USB CDC\n");
        // sensor.clearInterrupts();
        // sleep_ms(100);
    }

    // while (true)
    // {
    //     pwm_set_gpio_level(pwmPin, getLevel(100));
    //     counter++;
    //     if (counter == 101) {
    //         counter = 0;
    //         pwmPin = pwmPin == DRIVER_1_IN_1_PIN ? DRIVER_1_IN_2_PIN : DRIVER_1_IN_1_PIN;
    //         dirPin = dirPin == DRIVER_1_IN_1_PIN ? DRIVER_1_IN_2_PIN : DRIVER_1_IN_1_PIN;
    //         pwm_set_gpio_level(pwmPin, getLevel(100));
    //         pwm_set_gpio_level(dirPin, getLevel(0));
    //     }
    //     sleep_ms(20);
    // }
}

