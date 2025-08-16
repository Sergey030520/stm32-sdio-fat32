#include "led.h"
#include "gpio.h"
#include "memory_map.h"
#include "rcc.h"


void init_led()
{
    enable_and_reset_rcc(RCC_BUS_AHB1, RCC_AHB1ENR_GPIOEEN);
    GPIO_PinConfig_t pin_config = {
        .gpiox = GPIOE_REG,
        .pin = 0,
        .mode = GPIO_MODER_OUTPUT,
        .speed = GPIO_SPEED_100MHz,
        .pull = GPIO_PULL_NONE,
        .output = GPIO_OUTPUT_PUSHPULL,
        .af = 0,
    };

    for (uint8_t pin = 13; pin < 16; ++pin)
    {
        pin_config.pin = pin;
        set_gpio_conf(&pin_config);
        set_pin_gpio(GPIOE_REG, pin);
    }
}

void ledOn(int pin, int state)
{
    if (state)
        reset_pin_gpio(GPIOE_REG, pin);
    else
        set_pin_gpio(GPIOE_REG, pin);
}
