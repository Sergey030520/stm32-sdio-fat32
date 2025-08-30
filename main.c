#include <stdlib.h>
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "fpu.h"
#include "IRQ.h"
#include "timer.h"
#include "log.h"
#include "memory_map.h"
#include "board_pins.h"
#include "pool_memory.h"
#include "sd_fat32.h"
#include "sd_fat32_test.h"
#include "usart_board.h"



int init_rcc();
int init_board();



int main()
{
    int status = init_board();
    if (status != 0)
    {
        ledOn(13, 1);
        goto error;
    }
    LOG_INFO("Драйверы запущены!\r\n");
    status = init_sd_fat32();
    if(status != 0){
        LOG_INFO("Error init sd+fat32!(error=%d)\r\n", status);
        goto error;
    }
    
    sd_fat32_create_directory_test();
    // sd_fat32_write_file_test();
    // sd_fat32_write_and_read_test();
    sd_fat32_append_data_test();
    
    // ledOn(15, 1);
    while (1)
    {
        ledOn(14, 1);
        delay_timer(5000);
        ledOn(14, 0);
        delay_timer(5000);
    }

error:
    while (1)
        ;
    return 0;
}

int init_rcc()
{
    RCC_PLLConfig pll_config = {0};
    RCC_BusConfig bus_config = {0};
    RCC_SystemConfig system_config = {0};
    RCC_PeripheralClockConfig peripheral_conf = {0};

    int status = 0;

    pll_config.PLLM = 25;
    pll_config.PLLN = 336;
    pll_config.PLLP = RCC_PLLP_DIV2;
    pll_config.PLLQ = 7;

    pll_config.source = RCC_HSE;

    bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
    bus_config.apb1_prescaler = RCC_APB_DIV4;
    bus_config.apb2_prescaler = RCC_APB_DIV2;
    bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

    system_config.source = RCC_PLL;
    system_config.bus_config = &bus_config;
    system_config.pll_config = &pll_config;

    status = setup_system_config_rcc(&system_config);

    if (status != 0)
    {
        pll_config.PLLM = 16;
        pll_config.PLLN = 336;
        pll_config.PLLP = RCC_PLLP_DIV2;
        pll_config.PLLQ = 7;

        pll_config.source = RCC_HSI;

        bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
        bus_config.apb1_prescaler = RCC_APB_DIV4;
        bus_config.apb2_prescaler = RCC_APB_DIV2;
        bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

        status = setup_system_config_rcc(&system_config);
        if (status != 0)
        {
            bus_config.ahb_prescaler = RCC_NOT_DIV;
            bus_config.apb1_prescaler = RCC_NOT_DIV;
            bus_config.apb2_prescaler = RCC_NOT_DIV;

            system_config.source = RCC_HSI;
            system_config.pll_config = NULL;
            status = setup_system_config_rcc(&system_config);
        }
    }

    peripheral_conf.APB1 = RCC_APB1ENR_TIM2EN | RCC_APB1ENR_PWREN;
    peripheral_conf.APB2 = RCC_APB2ENR_USART1EN | RCC_APB2ENR_SYSCFGEN;
    peripheral_conf.AHB1 = RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_BKPSRAMEN;
    peripheral_conf.AHB2 = 0;
    peripheral_conf.AHB3 = 0;

    enable_gpio_clock_rcc(&peripheral_conf);
    return status;
}

int init_board()
{
    int status = 0;
    RCC_Frequencies rcc_clocks = {0};

    init_fpu();

    init_irq();

    status = init_rcc();

    init_led();

    if (status != 0)
    {
        ledOn(13, 1);
        return -1;
    }

    init_timer();

    init_uart();

    stm_init_log(usart_adapter);

    return 0;
}


