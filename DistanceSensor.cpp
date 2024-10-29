#include "DistanceSensor.hpp"

// https://www.st.com/resource/en/application_note/an4545-vl6180x-basic-ranging-application-note-stmicroelectronics.pdf

void DistanceSensor::writeByte(uint16_t reg, uint8_t data)
{
    uint8_t buf[3];
    buf[0] = (reg >> 8) & 0xFF; // MSB of register address
    buf[1] = reg & 0xFF;        // LSB of register address
    buf[2] = data & 0xFF;
    i2c_write_blocking(i2cInst, i2cAddress, buf, 3, false);
}

uint8_t DistanceSensor::readByte(uint16_t reg)
{
    uint8_t dataWrite[2];
    uint8_t dataRead[1];
    dataWrite[0] = (reg >> 8) & 0xFF; // MSB of register address
    dataWrite[1] = reg & 0xFF;        // LSB of register address

    i2c_write_blocking(i2cInst, i2cAddress, dataWrite, 2, false);
    i2c_read_blocking(i2cInst, i2cAddress, dataRead, 1, false);
    return dataRead[0];
}

DistanceSensor::DistanceSensor(int sdaPin, int sclPin, i2c_inst *i2cInst)
    : sdaPin(sdaPin), sclPin(sclPin), i2cInst(i2cInst)
{
    uint8_t reset = readByte(SYSTEM__FRESH_OUT_OF_RESET);

    if (reset == 1)
    {
        // Mandatory : private registers
        writeByte(0x0207, 0x01);
        writeByte(0x0208, 0x01);
        writeByte(0x0096, 0x00);
        writeByte(0x0097, 0xfd);
        writeByte(0x00e3, 0x01);
        writeByte(0x00e4, 0x03);
        writeByte(0x00e5, 0x02);
        writeByte(0x00e6, 0x01);
        writeByte(0x00e7, 0x03);
        writeByte(0x00f5, 0x02);
        writeByte(0x00d9, 0x05);
        writeByte(0x00db, 0xce);
        writeByte(0x00dc, 0x03);
        writeByte(0x00dd, 0xf8);
        writeByte(0x009f, 0x00);
        writeByte(0x00a3, 0x3c);
        writeByte(0x00b7, 0x00);
        writeByte(0x00bb, 0x3c);
        writeByte(0x00b2, 0x09);
        writeByte(0x00ca, 0x09);
        writeByte(0x0198, 0x01);
        writeByte(0x01b0, 0x17);
        writeByte(0x01ad, 0x00);
        writeByte(0x00ff, 0x05);
        writeByte(0x0100, 0x05);
        writeByte(0x0199, 0x05);
        writeByte(0x01a6, 0x1b);
        writeByte(0x01ac, 0x3e);
        writeByte(0x01a7, 0x1f);
        writeByte(0x0030, 0x00);

        // Recommended : Public registers - See data sheet for more detail
        writeByte(SYSTEM__MODE_GPIO1, 0x10);         // Enables polling for ‘New Sample ready’ when measurement completes
        writeByte(SYSRANGE__THRESH_LOW, 0x30);       // Set the averaging sample period (compromise between lower noise and increased execution time)
        writeByte(SYSALS__ANALOGUE_GAIN, 0x46);      // Sets the light and dark gain (upper nibble). Dark gain should not be changed.
        writeByte(SYSRANGE__VHV_REPEAT_RATE, 0xFF);  // sets the # of range measurements after which auto calibration of system is performed
        writeByte(SYSALS__INTEGRATION_PERIOD, 0x63); // Set ALS integration time to 100ms
        writeByte(SYSRANGE__VHV_RECALIBRATE, 0x01);  // perform a single temperature calibratio of the ranging sensor

        // Optional: Public registers - See data sheet for more detail
        writeByte(SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09); // Set default ranging inter-measurement period to 100ms
        writeByte(SYSALS__INTERMEASUREMENT_PERIOD, 0x31);   // Set default ALS inter-measurement period to 500ms
        writeByte(SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24);     // Configures interrupt on ‘New Sample Ready threshold event’

        writeByte(SYSTEM__FRESH_OUT_OF_RESET, 0x00);
    }
}

void DistanceSensor::startRange()
{
    writeByte(SYSRANGE__START, 0x01);
}

void DistanceSensor::pollRange()
{
    uint8_t status;
    uint8_t range_status;

    // check the status
    status = readByte(RESULT__INTERRUPT_STATUS_GPIO);
    range_status = status & 0x07;

    // wait for new measurement ready status
    while (range_status != 0x04)
    {
        status = readByte(RESULT__INTERRUPT_STATUS_GPIO);
        range_status = status & 0x07;
    }
}

uint8_t DistanceSensor::readRange()
{
    uint8_t range = readByte(RESULT__RANGE_VAL);
    return range;
}

void DistanceSensor::clearInterrupts()
{
    writeByte(SYSTEM__INTERRUPT_CLEAR, 0x07);
}

// int get_distance(vl6180 handle)
// {

//     int range;
//     start_range(handle);
//     poll_range(handle);

//     range = read_byte(handle, VL6180X_RESULT_RANGE_VAL);
//     clear_interrupts(handle);
//     return range;
// }