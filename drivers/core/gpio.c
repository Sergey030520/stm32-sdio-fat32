#include "gpio.h"
#include "led.h"
#include <stdio.h>


void reset_gpio_port(GPIO_Type *gpiox, uint16_t pin)
{
    gpiox->MODER &= ~GPIO_MASK_MODE(pin);
    gpiox->OTYPER &= ~GPIO_MASK_OTYPER(pin);
    gpiox->SPEEDR &= ~GPIO_MASK_SPEED(pin);
    gpiox->PUPDR &= ~GPIO_MASK_PUPDR(pin);

    if (pin > 7)
        gpiox->AFRH &= ~(0xF << (4 * (pin % 8)));
    else
        gpiox->AFRL &= ~(0xf << (pin * 4));
}

void set_gpio_conf(GPIO_PinConfig_t *config)
{
    if (config == NULL || config->gpiox == NULL) {
        return;
    }

    GPIO_Type *gpiox = config->gpiox;
    uint16_t pin = config->pin;

    reset_gpio_port(gpiox, pin);

    gpiox->MODER  |= GPIO_SET_MODE(pin, config->mode);
    gpiox->OTYPER |= GPIO_SET_OTYPER(pin, config->output);
    gpiox->SPEEDR |= GPIO_SET_SPEED(pin, config->speed);
    gpiox->PUPDR  |= GPIO_SET_PUPDR(pin, config->pull);

    if (pin > 7)
        gpiox->AFRH |= GPIO_SET_AF(pin, config->af);
    else
        gpiox->AFRL |= GPIO_SET_AF(pin, config->af);
}


void set_pin_gpio(GPIO_Type *gpiox, uint16_t pin)
{
    gpiox->BSRR = GPIO_SET_BS(pin);
}

void reset_pin_gpio(GPIO_Type *gpiox, uint16_t pin)
{
    gpiox->BSRR = GPIO_SET_BR(pin);
}