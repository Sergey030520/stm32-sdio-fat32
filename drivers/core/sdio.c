#include "sdio.h"
#include "memory_map.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "utils.h"
#include "rcc.h"
#include "board_pins.h"



#define TIMELOAD 1000000

// 0x40012C00UL
SDIO_Type *SDIO = SDIO_REG;
sdio_callback_t *callback;

int configure_power_SDIO(SDIO_Power_State state)
{
    if (state == POWER_ON)
    {
        SDIO->POWER = SDIO_PWRCTRL_ON;
        int timeout = 10000;
        while (1)
        {
            if ((SDIO->POWER & SDIO_PWRCTRL_ON) == SDIO_PWRCTRL_ON)
                break;
            else if (--timeout == 0)
            {
                return -1;
            }
        }

        delay_timer(1000);
    }
    else
    {
        SDIO->POWER = SDIO_PWRCTRL_OFF;
        delay_timer(1000);
    }
    return 0;
}

uint8_t sdio_calc_clkcr_div(uint32_t apb2_freq_hz, uint32_t target_freq_hz)
{
    if (target_freq_hz == 0)
    {
        return 0;
    }

    // SDIO_CK = SDIOCLK / (CLKDIV + 2)
    int32_t div = (int32_t)(apb2_freq_hz / target_freq_hz) - 2;

    if (div < 0)
    {
        div = 0;
    }
    else if (div > 0xFF)
    {
        div = 0xFF;
    }

    return (uint8_t)div;
}

int init_sdio()
{
    GPIO_PinConfig_t pin_conf = {
    .gpiox  = GPIOC_REG,          
    .pin    = 0,                   
    .mode   = GPIO_MODER_ALTERNATE,  
    .speed  = GPIO_SPEED_100MHz,    
    .pull   = GPIO_PULL_UP,          
    .output = GPIO_OUTPUT_PUSHPULL,  
    .af     = GPIO_AF12               
};

// Настройка пина через структуру
set_gpio_conf(&pin_conf);


    enable_and_reset_rcc(RCC_BUS_AHB1, RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN);
    enable_and_reset_rcc(RCC_BUS_APB2, RCC_APB2ENR_SDIOEN);

    // setup gpio
    // PC[8-11] - Data
    for (uint16_t pin = SDIO_D0_PIN; pin <= SDIO_D3_PIN; ++pin)
    {
        pin_conf.pin = pin;
        set_gpio_conf(&pin_conf);
    }

    pin_conf.pin = SDIO_CLK_PIN;
    
    // PC12 - CLK
    set_gpio_conf(&pin_conf);

    pin_conf.gpiox = GPIOD_REG;
    pin_conf.pin = SDIO_CMD_PIN;

    // PD2 - CMD
    set_gpio_conf(&pin_conf);

    // Power off
    configure_power_SDIO(POWER_OFF);

    delay_timer(10);

    if (configure_power_SDIO(POWER_ON))
    {
        return -1;
    }
    delay_timer(10);


    // CLKR CLEAR
    SDIO->CLKCR = 0;

    // CLKR_SET
    set_bus_width_sdio(WIDE_BUS_1BIT_MODE);


    uint8_t div = sdio_calc_clkcr_div(get_pll48clk_freq(), FREQ_400kHZ);    
    
    set_clock_frequency_sdio(div);
    
    delay_timer(10);

    // SDIO_MASK_CCRCFAILIE | SDIO_MASK_DCRCFAILIE | SDIO_MASK_RXOVERRIE | SDIO_STA_TXUNDERR;
    return 0;
}

void send_command_sdio(uint32_t command, uint32_t arg, uint32_t format_resp)
{
    SDIO->ARG = arg;
    SDIO->CMD = command | format_resp | SDIO_CMD_CPSMEN;
}

void clear_flags_sdio(uint32_t mask)
{
    SDIO->ICR = mask;
}

void SDIO_IRQHandler()
{
    // Передать данные по всем ошибкам в log
    if (SDIO->STA & SDIO_STA_CCRCFAIL)
    {

        SDIO->ICR |= SDIO_ICR_CCRCFAILC;
    }
    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        SDIO->ICR |= SDIO_ICR_DCRCFAILC;
    }
    if (SDIO->STA & SDIO_STA_TXUNDERR)
    {
        SDIO->ICR |= SDIO_ICR_TXUNDERRC;
    }
    if (SDIO->STA & SDIO_STA_RXOVERR)
    {
        SDIO->ICR |= SDIO_ICR_RXOVERRC;
    }
}

void sdio_read_long_resp(uint32_t *data, uint16_t size)
{
    if (size < 4)
    {
        return;
    }
    data[0] = SDIO->RESP[0];
    data[1] = SDIO->RESP[1];
    data[2] = SDIO->RESP[2];
    data[3] = SDIO->RESP[3];
}

void sdio_config_data(uint32_t data_control, uint32_t length, uint32_t timeout)
{
    SDIO->DTIMER = timeout;
    SDIO->DLEN = length;
    SDIO->DCTRL = data_control;
}

void clear_flag_sdio(uint32_t flag)
{
    SDIO->ICR |= flag;
}

void set_bus_width_sdio(WIDE_BUS_SDIO wide_bus)
{
    SDIO->CLKCR &= ~SDIO_MASK_BUSWIDTH;
    SDIO->CLKCR |= SDIO_CLKCR_WIDBUS((uint32_t)wide_bus);
}

void set_clock_frequency_sdio(uint8_t div)
{
    SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN;
    SDIO->CLKCR &= ~SDIO_MASK_CLKDIV;
    SDIO->CLKCR |= SDIO_CLKCR_CLKDIV(div);
    SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
}

void set_irq_sdio(uint32_t mask, sdio_callback_t handler)
{
    callback = &handler;
    SDIO->MASK |= mask;
}
void reset_irq_sdio(uint32_t mask)
{
    SDIO->ICR |= mask;
}
