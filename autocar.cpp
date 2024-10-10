#include "pico/stdlib.h"
#include "hardware/pwm.h"

#ifndef SLEEP_PIN
#define SLEEP_PIN 4
#endif
#include <hardware/clocks.h>
#ifndef FAULT_PIN
#define FAULT_PIN 3
#endif
#ifndef IN_1_PIN
#define IN_1_PIN 1
#endif
#ifndef IN_2_PIN
#define IN_2_PIN 2
#endif
#ifndef MAX_SPEED
#define MAX_SPEED 10000
#endif

// // Perform initialisation
// int pico_led_init(void)
// {
//     // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
//     // so we can use normal GPIO functionality to turn the led on and off
//     gpio_init(PICO_DEFAULT_LED_PIN);
//     gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
//     return PICO_OK;
// }

// // Turn the led on or off
// void pico_set_led(bool led_on)
// {
//     // Just set the GPIO on or off
//     gpio_put(PICO_DEFAULT_LED_PIN, led_on);
// }

/// @brief Initializes pins
/// @return PICO_OK if ok
int init_pins()
{
    gpio_init(SLEEP_PIN);
    gpio_set_dir(SLEEP_PIN, GPIO_OUT);
    gpio_init(FAULT_PIN);
    gpio_set_dir(FAULT_PIN, GPIO_IN);
    // gpio_init(IN_1_PIN);
    // gpio_set_dir(IN_1_PIN, GPIO_FUNC_PWM);
    // gpio_init(IN_2_PIN);
    // gpio_set_dir(IN_2_PIN, GPIO_FUNC_PWM);
    return PICO_OK;
}

/// @brief Sets up pin for pwm output
/// @param pin Pin number
/// @param freq Frequency in Hz
void set_pwm_pin(uint pin, uint freq)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();

    float div = (float)clock_get_hz(clk_sys) / (freq * MAX_SPEED);
    pwm_config_set_clkdiv(&config, div);
    pwm_config_set_wrap(&config, MAX_SPEED);
    pwm_init(slice_num, &config, true); // start the pwm running according to the config
};

/// @brief Sets up 
/// @return PICO_OK if ok
int setup_pwm()
{
    set_pwm_pin(IN_1_PIN, 20000);
    set_pwm_pin(IN_2_PIN, 20000);
    return PICO_OK;
}

/// @brief Gets level for pwm_set_gpio_level function
/// @param dutyCycle Duty cycle 0-100
/// @return
static int getLevel(float dutyCycle)
{
    return MAX_SPEED * dutyCycle / 100;
}

int main()
{
    // initialize pins
    int rc = init_pins();
    hard_assert(rc == PICO_OK);

    rc = setup_pwm();
    hard_assert(rc == PICO_OK);

    // enable drv8833
    gpio_put(SLEEP_PIN, 1);

    uint8_t pwmPin = IN_1_PIN;
    uint8_t dirPin = IN_2_PIN;
    uint16_t counter = 0;

    while (true)
    {
        pwm_set_gpio_level(pwmPin, getLevel(100));
        counter++;
        if (counter == 101) {
            counter = 0;
            pwmPin = pwmPin == IN_1_PIN ? IN_2_PIN : IN_1_PIN;
            dirPin = dirPin == IN_1_PIN ? IN_2_PIN : IN_1_PIN;
            pwm_set_gpio_level(pwmPin, getLevel(100));
            pwm_set_gpio_level(dirPin, getLevel(0));
        }
        sleep_ms(20);
    }
}
