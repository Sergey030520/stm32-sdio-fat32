#include <stdint.h>

#pragma once

typedef struct
{
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t SPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFRL;
    uint32_t AFRH;
} GPIO_Type;

typedef enum
{
    GPIO_SPEED_2MHz = 0x0,  // Input mode
    GPIO_SPEED_25MHz = 0x1, // Output mode
    GPIO_SPEED_50MHz = 0x2, // Output mode
    GPIO_SPEED_100MHz = 0x3 // Output mode
} GPIO_MODE_Speed;

typedef enum
{
    GPIO_PULL_NONE = 0x0, // No pull-up, pull-down
    GPIO_PULL_UP = 0x1,   // Pull-up
    GPIO_PULL_DOWN = 0x2  // Pull-down
} GPIO_PULL_MODE;

typedef enum
{
    GPIO_OUTPUT_PUSHPULL = 0x0, // Output push-pull (reset state)
    GPIO_OUTPUT_OPENDRAIN = 0x1 // Output open-drain
} GPIO_OUTPUT_TYPE;

typedef enum
{
    GPIO_MODER_INPUT = 0x0,     // 00: Input (reset state)
    GPIO_MODER_OUTPUT = 0x1,    // 01: General purpose output
    GPIO_MODER_ALTERNATE = 0x2, // 10: Alternate function
    GPIO_MODER_ANALOG = 0x3     // 11: Analog mode
} GPIO_MODER_MODE;

typedef enum
{
    GPIO_AF0 = 0x0,
    GPIO_AF1 = 0x1,
    GPIO_AF2 = 0x2,
    GPIO_AF3 = 0x3,
    GPIO_AF4 = 0x4,
    GPIO_AF5 = 0x5,
    GPIO_AF6 = 0x6,
    GPIO_AF7 = 0x7,
    GPIO_AF8 = 0x8,
    GPIO_AF9 = 0x9,
    GPIO_AF10 = 0xA,
    GPIO_AF11 = 0xB,
    GPIO_AF12 = 0xC,
    GPIO_AF13 = 0xD,
    GPIO_AF14 = 0xE,
    GPIO_AF15 = 0xF,

} GPIO_AFx;



#define GPIO_SET_MODE(pin, mode) ((uint32_t)mode << (pin * 2))
#define GPIO_MASK_MODE(pin) ((uint32_t)0b11 << (pin * 2))

#define GPIO_SET_OTYPER(pin, type_output) ((uint32_t)type_output << pin)
#define GPIO_MASK_OTYPER(pin) ((uint32_t)0x1 << pin)

#define GPIO_SET_SPEED(pin, speed) ((uint32_t)speed << (pin * 2))
#define GPIO_MASK_SPEED(pin) ((uint32_t)0b11 << (pin * 2))

#define GPIO_SET_PUPDR(pin, value) ((uint32_t)value << (pin * 2))
#define GPIO_MASK_PUPDR(pin) ((uint32_t)0b11 << (pin * 2))

#define GPIO_SET_BR(pin) ((uint32_t)0x1 << (16 + pin))
#define GPIO_SET_BS(pin) ((uint32_t)0x1 << pin)

#define GPIO_SET_AF(pin, af) ((uint32_t)af << ((pin % 8) * 4))
#define GPIO_MASK_AF(pin) ((uint32_t)0b1111 << ((pin % 8) * 4))



typedef struct {
    GPIO_Type *gpiox;
    uint16_t pin;
    GPIO_MODER_MODE mode;       
    GPIO_MODE_Speed speed;      
    GPIO_PULL_MODE pull;        
    GPIO_OUTPUT_TYPE output;    
    GPIO_AFx af;                
} GPIO_PinConfig_t;


void __attribute__((used)) reset_gpio_port(GPIO_Type *gpiox, uint16_t pin);
void __attribute__((used)) set_gpio_conf(GPIO_PinConfig_t *config);

void set_pin_gpio(GPIO_Type *port, uint16_t pin);
void reset_pin_gpio(GPIO_Type *port, uint16_t pin);
