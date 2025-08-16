#include "rcc.h"
#include "flash.h"
#include "memory_map.h"
#include <stdlib.h>
#include "led.h"
#include "utils.h"

RCC_Type *RCC = RCC_REG;

static inline uint32_t decode_pllp(uint8_t pllp_code);
void disable_all_GPIO_clocks_rcc();
void set_bus_dividers_rcc(RCC_BusConfig *bus_conf);
int configure_pll_rcc(RCC_PLLConfig *config);
int select_sys_clock(RCC_ClockSource src);
int switch_to_hsi_rcc();
int switch_to_hse_rcc();
int switch_to_pll_rcc(RCC_PLLConfig *pll_config);

void disable_all_GPIO_clocks_rcc()
{
    // AHBx
    RCC->AHB1ENR = 0;
    RCC->AHB2ENR = 0;
    RCC->AHB3ENR = 0;
    // APBx
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
}

int setup_system_config_rcc(RCC_SystemConfig *config)
{
    int status = 0;

    if (config == NULL)
    {
        return -1;
    }

    disable_all_GPIO_clocks_rcc();

    RCC->CR |= RCC_CR_CSSON;

    if (config->source == RCC_HSE)
    {
        status = switch_to_hse_rcc();
    }
    else if (config->source == RCC_HSI)
    {
        status = switch_to_hsi_rcc();
    }
    else if (config->source == RCC_PLL)
    {
        status = switch_to_pll_rcc(config->pll_config);
    }
    else
    {
        return -3;
    }
    if (status != 0)
        return status;

    if (config->bus_config != NULL)
    {
        set_bus_dividers_rcc(config->bus_config);
    }
    return 0;
}

int configure_pll_rcc(RCC_PLLConfig *config)
{
    if (config == NULL)
    {
        return -1;
    }
    if (RCC->CR & RCC_CR_PLLON)
    {
        return -2;
    }
    if (config->source != RCC_HSE && config->source != RCC_HSI)
    {
        return -3;
    }

    // Сбрасываем старые значения PLL
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_MASK | RCC_PLLCFGR_PLLN_MASK | RCC_PLLCFGR_PLLQ_MASK | RCC_PLLCFGR_PLLP_MASK);

    // Настраиваем PLL
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM(config->PLLM);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ(config->PLLQ);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN(config->PLLN);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP(config->PLLP);

    // Источник PLL
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_MASK;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC(config->source);

    return 0;
}

void set_flash_latency(uint32_t hclk_freq)
{
    FLASH_Type *flash = (FLASH_Type *)FLASH_REG;
    flash->ACR &= ~0x7;
    if (hclk_freq <= FREQ_30MHZ) // 0–30 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_0);
    else if (hclk_freq <= FREQ_60MHZ) // 30–60 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_1);
    else if (hclk_freq <= FREQ_90MHZ) // 60–90 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_2);
    else if (hclk_freq <= FREQ_120MHZ) // 90–120 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_3);
    else if (hclk_freq <= FREQ_150MHZ) // 120–150 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_4);
    else if (hclk_freq <= FREQ_168MHZ) // 150–168 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_5);
    else if (hclk_freq <= FREQ_180MHZ) // 168–180 MHz
        flash->ACR |= FLASH_SET_LATENCY(FLASH_LATENCY_6);
}

void set_bus_dividers_rcc(RCC_BusConfig *bus_conf)
{
    if (bus_conf == NULL)
    {
        return;
    }
    RCC->CFGR &= ~(RCC_CFGR_PPRE1_MASK | RCC_CFGR_PPRE2_MASK | RCC_CFGR_HPRE_MASK | RCC_CFGR_RTCPRE_MASK);
    RCC->CFGR |= RCC_CFGR_PPRE1(bus_conf->apb1_prescaler);
    RCC->CFGR |= RCC_CFGR_PPRE2(bus_conf->apb2_prescaler);
    RCC->CFGR |= RCC_CFGR_HPRE(bus_conf->ahb_prescaler);
    RCC->CFGR |= RCC_CFGR_RTCPRE(bus_conf->rtc_prescaler);
}

int select_sys_clock(RCC_ClockSource src)
{
    if (src == RCC_HSI)
    {
        if (!(RCC->CR & RCC_CR_HSIRDY))
            return -1;
    }
    else if (src == RCC_HSE)
    {
        if (!(RCC->CR & RCC_CR_HSERDY))
            return -2;
    }
    else if (src == RCC_PLL)
    {
        if (!(RCC->CR & RCC_CR_PLLRDY))
            return -3;
    }
    else
    {
        return -4;
    }

    RCC->CFGR &= ~RCC_CFGR_SW(0x3);
    RCC->CFGR |= RCC_CFGR_SW(src);

    int counter = 10000000000;
    while ((RCC->CFGR & RCC_CFGR_SWS(0x3)) != RCC_CFGR_SWS(src))
    {
        if (--counter == 0)
            return -5;
    }
    return 0;
}

int switch_to_hsi_rcc()
{
    if (!(RCC->CR & RCC_CR_HSION))
    {
        RCC->CR |= RCC_CR_HSION;
        if (wait_for_flags(&RCC->CR, RCC_CR_HSIRDY, 100000000) != 0)
        {
            return -1;
        }
    }

    if (select_sys_clock(RCC_HSI) != 0)
        return -2;

    if (RCC->CR & RCC_CR_HSEON)
    {
        RCC->CR &= ~RCC_CR_HSEON;
    }
    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
    }
    return 0;
}

int switch_to_hse_rcc()
{
    if (!(RCC->CR & RCC_CR_HSEON))
    {
        RCC->CR |= RCC_CR_HSEON;
        if (wait_for_flags(&RCC->CR, RCC_CR_HSERDY, 100000000) != 0)
        {
            return -1;
        }
    }

    if (select_sys_clock(RCC_HSE) != 0)
        return -2;

    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
    }
    if (RCC->CR & RCC_CR_HSION)
    {
        RCC->CR &= ~RCC_CR_HSION;
    }
    return 0;
}

int switch_to_pll_rcc(RCC_PLLConfig *pll_config)
{
    if (!pll_config)
        return -1;

    const uint32_t TIMEOUT = 1000000;

    if (pll_config->source == RCC_HSE)
    {
        if (!(RCC->CR & RCC_CR_HSEON))
        {
            RCC->CR |= RCC_CR_HSEON;
            if (wait_for_flags(&RCC->CR, RCC_CR_HSERDY, TIMEOUT) != 0)
                return -2;
        }
    }
    else
    {
        if (!(RCC->CR & RCC_CR_HSION))
        {
            RCC->CR |= RCC_CR_HSION;
            if (wait_for_flags(&RCC->CR, RCC_CR_HSIRDY, TIMEOUT) != 0)
                return -3;
        }
    }

    uint32_t input_freq = (pll_config->source == RCC_HSE) ? RCC_HSE_FREQ : RCC_HSI_FREQ;
    uint32_t pllp_div = decode_pllp(pll_config->PLLP);
    uint32_t target_sysclk = ((input_freq / pll_config->PLLM) * pll_config->PLLN) / pllp_div;

    RCC_Frequencies current_freq;
    get_clock_frequencies(&current_freq);

    if (target_sysclk > current_freq.SYSCLK_Freq)
        set_flash_latency(target_sysclk);

    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
        uint32_t timeout = TIMEOUT;
        while (RCC->CR & RCC_CR_PLLRDY)
        {
            if (--timeout == 0)
                return -4;
        }
    }

    if (configure_pll_rcc(pll_config) != 0)
        return -5;

    RCC->CR |= RCC_CR_PLLON;
    if (wait_for_flags(&RCC->CR, RCC_CR_PLLRDY, TIMEOUT) != 0)
        return -6;

    if (select_sys_clock(RCC_PLL) != 0)
        return -7;

    uint32_t timeout = TIMEOUT;
    while ((RCC->CFGR & RCC_CFGR_SWS(0x3)) != RCC_CFGR_SWS(RCC_PLL))
    {
        if (--timeout == 0)
            return -8;
    }

    if (target_sysclk < current_freq.SYSCLK_Freq)
        set_flash_latency(target_sysclk);

    return 0;
}

void enable_gpio_clock_rcc(RCC_PeripheralClockConfig *config)
{
    RCC->APB1ENR = config->APB1;
    RCC->APB2ENR = config->APB2;
    RCC->AHB1ENR = config->AHB1;
    RCC->AHB2ENR = config->AHB2;
    RCC->AHB3ENR = config->AHB3;
}

static inline uint32_t decode_pllp(uint8_t pllp_code)
{
    return (pllp_code + 1) * 2;
}

void get_clock_frequencies(RCC_Frequencies *freq)
{
    if (freq == NULL)
        return;

    // Получаем источник системного тактирования
    uint32_t clock_source = ((RCC->CFGR & RCC_CFGR_SWS(0x3)) >> 2);
    // Определяем частоту SYSCLK
    switch (clock_source)
    {
    case RCC_HSI:
        freq->SYSCLK_Freq = RCC_HSI_FREQ;
        break;
    case RCC_HSE:
        freq->SYSCLK_Freq = RCC_HSE_FREQ;
        break;
    case RCC_PLL:
        uint32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_MASK) >> 0;
        uint32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_MASK) >> 6;
        uint32_t pllp = decode_pllp(((RCC->PLLCFGR & RCC_PLLCFGR_PLLP_MASK) >> 16));
        uint32_t pll_src = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_MASK) >> 22;
        uint32_t pll_input_freq = ((pll_src == RCC_HSI) ? RCC_HSI_FREQ : RCC_HSE_FREQ);

        freq->SYSCLK_Freq = (pll_input_freq / pllm) * plln / pllp;
        break;
    }

    // Вычисляем HCLK (AHB)
    uint32_t hpre = (RCC->CFGR & RCC_CFGR_HPRE_MASK) >> 4;
    freq->HCLK_Freq = freq->SYSCLK_Freq >> ((hpre > 7) ? (hpre - 7) : 0);

    // Вычисляем APB1
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1_MASK) >> 10;
    freq->APB1_Freq = freq->HCLK_Freq >> ((ppre1 > 3) ? (ppre1 - 3) : 0);

    // Вычисляем APB2
    uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2_MASK) >> 13;
    freq->APB2_Freq = freq->HCLK_Freq >> ((ppre2 > 3) ? (ppre2 - 3) : 0);
}

void enable_and_reset_rcc(RCC_Bus bus, uint32_t periph_mask)
{
    switch (bus)
    {
    case RCC_BUS_AHB1:
        RCC->AHB1ENR |= periph_mask;
        RCC->AHB1RSTR |= periph_mask;
        RCC->AHB1RSTR &= ~periph_mask;
        break;

    case RCC_BUS_AHB2:
        RCC->AHB2ENR |= periph_mask;
        RCC->AHB2RSTR |= periph_mask;
        RCC->AHB2RSTR &= ~periph_mask;
        break;

    case RCC_BUS_AHB3:
        RCC->AHB3ENR |= periph_mask;
        RCC->AHB3RSTR |= periph_mask;
        RCC->AHB3RSTR &= ~periph_mask;
        break;

    case RCC_BUS_APB1:
        RCC->APB1ENR |= periph_mask;
        RCC->APB1RSTR |= periph_mask;
        RCC->APB1RSTR &= ~periph_mask;
        break;

    case RCC_BUS_APB2:
        RCC->APB2ENR |= periph_mask;
        // RCC->APB2RSTR |= periph_mask;
        // RCC->APB2RSTR &= ~periph_mask;
        break;
    }
}

void get_rcc_clock_dividers(RCC_BusConfig *config)
{
     // Вычисляем HCLK
    config->ahb_prescaler = (RCC->CFGR & RCC_CFGR_HPRE_MASK) >> 4;

    // Вычисляем APB1
    config->apb1_prescaler = (RCC->CFGR & RCC_CFGR_PPRE1_MASK) >> 10;

    // Вычисляем APB2
    config->apb2_prescaler = (RCC->CFGR & RCC_CFGR_PPRE2_MASK) >> 13;

    // Вычисляем RTC
    config->rtc_prescaler = (RCC->CFGR & RCC_CFGR_RTCPRE_MASK) >> 16;
}

uint32_t get_pll48clk_freq()
{
    uint32_t pll_src_freq;
    
    uint32_t pll_src = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_MASK) >> 22;
    if (pll_src == 0)
        pll_src_freq = RCC_HSI_FREQ; // 16 MHz
    else
        pll_src_freq = RCC_HSE_FREQ; // 8 MHz (или смотри конфиг)

    uint32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_MASK) >> 0;
    uint32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_MASK) >> 6;
    uint32_t pllq = (RCC->PLLCFGR & RCC_PLLCFGR_PLLQ_MASK) >> 24;

    if (pllm == 0 || pllq == 0)
        return 0; 


    uint32_t pll48clk = (pll_src_freq / pllm) * plln / pllq;

    return pll48clk;
}
