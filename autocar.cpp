#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "Globals.hpp"
#include "Driver.hpp"

int main()
{
    stdio_usb_init();

    Driver driver1 = Driver(
        DRIVER_1_IN_1_PIN,
        DRIVER_1_IN_2_PIN,
        DRIVER_1_IN_3_PIN,
        DRIVER_1_IN_4_PIN,
        DRIVER_1_SLEEP_PIN,
        DRIVER_1_FAULT_PIN
    );

    Driver driver2 = Driver(
        DRIVER_2_IN_1_PIN,
        DRIVER_2_IN_2_PIN,
        DRIVER_2_IN_3_PIN,
        DRIVER_2_IN_4_PIN,
        DRIVER_2_SLEEP_PIN,
        DRIVER_2_FAULT_PIN
    );

    driver1.enable();

    driver1.motors[0].setSpeed(50);
    driver1.motors[1].setSpeed(50);

    driver2.enable();

    driver2.motors[0].setSpeed(50);
    driver2.motors[1].setSpeed(50);

    while(true) {
        
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
