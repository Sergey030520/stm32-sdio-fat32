#include <stdint.h>

#pragma once

typedef enum
{
    FREQ_32kHZ = 32000,
    FREQ_400kHZ = 400000,
    FREQ_8MHZ = 8000000,
    FREQ_16MHZ = 16000000,
    FREQ_24MHZ = 24000000,
    FREQ_25MHZ = 25000000,
    FREQ_30MHZ = 30000000,
    FREQ_48MHZ = 48000000,
    FREQ_60MHZ = 60000000,
    FREQ_72MHZ = 72000000,
    FREQ_84MHZ = 84000000,
    FREQ_90MHZ = 90000000,
    FREQ_120MHZ = 120000000,
    FREQ_150MHZ = 150000000,
    FREQ_168MHZ = 168000000,
    FREQ_180MHZ = 180000000
} Frequency;

#define RCC_HSI_FREQ FREQ_16MHZ
#define RCC_HSE_FREQ FREQ_25MHZ
#define RCC_LSI_FREQ FREQ_32kHZ
#define RCC_LSE_FREQ 32768

typedef struct
{
    uint32_t CR;
    uint32_t PLLCFGR;
    uint32_t CFGR;
    uint32_t CIR;
    uint32_t AHB1RSTR;
    uint32_t AHB2RSTR;
    uint32_t AHB3RSTR;
    uint32_t reserv;
    uint32_t APB1RSTR;
    uint32_t APB2RSTR;
    uint32_t reserv1[2];
    uint32_t AHB1ENR;
    uint32_t AHB2ENR;
    uint32_t AHB3ENR;
    uint32_t reserv2;
    uint32_t APB1ENR;
    uint32_t APB2ENR;
    uint32_t reserv3[2];
    uint32_t AHB1LPENR;
    uint32_t AHB2LPENR;
    uint32_t AHB3LPENR;
    uint32_t reserv4;
    uint32_t APB1LPENR;
    uint32_t APB2LPENR;
    uint32_t reserv5[2];
    uint32_t BDCR;
    uint32_t CSR;
    uint32_t reserv6[2];
    uint32_t SSCGR;
    uint32_t PLLI2SCFGR;
} RCC_Type;

// CR
#define RCC_CR_PLLI2SRDY (0x1 << 27)
#define RCC_CR_PLLI2SON (0x1 << 26)
#define RCC_CR_PLLRDY (0x1 << 25)
#define RCC_CR_PLLON (0x1 << 24)
#define RCC_CR_CSSON (0x1 << 19)
#define RCC_CR_HSEBYP (0x1 << 18)
#define RCC_CR_HSERDY (0x1 << 17)
#define RCC_CR_HSEON (0x1 << 16)
#define RCC_CR_HSIRDY (0x1 << 1)
#define RCC_CR_HSION (0x1 << 0)

// PLLCFGR
#define RCC_PLLCFGR_PLLQ(value) (value << 24)
#define RCC_PLLCFGR_PLLQ_MASK (0xf << 24)
#define RCC_PLLCFGR_PLLSRC(source) (source << 22)
#define RCC_PLLCFGR_PLLSRC_MASK (0x1 << 22)
#define RCC_PLLCFGR_PLLP(value) (value << 16)
#define RCC_PLLCFGR_PLLP_MASK (0x3 << 16)
#define RCC_PLLCFGR_PLLN(value) (value << 6)
#define RCC_PLLCFGR_PLLN_MASK (0x1FF << 6)
#define RCC_PLLCFGR_PLLM(value) (value << 0)
#define RCC_PLLCFGR_PLLM_MASK (0x3F << 0)

typedef enum
{
    RCC_PLL_MUL2 = 0x0,
    RCC_PLL_MUL3,
    RCC_PLL_MUL4,
    RCC_PLL_MUL5,
    RCC_PLL_MUL6,
    RCC_PLL_MUL7,
    RCC_PLL_MUL8,
    RCC_PLL_MUL9,
    RCC_PLL_MUL10,
    RCC_PLL_MUL11,
    RCC_PLL_MUL12,
    RCC_PLL_MUL13,
    RCC_PLL_MUL14,
    RCC_PLL_MUL15,
    RCC_PLL_MUL16
} RCC_PLLMul;

typedef enum
{
    RCC_PLLP_DIV2 = 0x0,
    RCC_PLLP_DIV4 = 0x1,
    RCC_PLLP_DIV6 = 0x2,
    RCC_PLLP_DIV8 = 0x3
} RCC_PLLP_Divider;

// CFGR

#define RCC_CFGR_MCO2(source) (source << 30)
#define RCC_CFGR_MCO2PRE(value) (value << 27)
#define RCC_CFGR_MCO2PRE_MASK (0x7 << 27)
#define RCC_CFGR_MCO1PRE(value) (value << 24)
#define RCC_CFGR_MCO1PRE_MASK (0x7 << 24)
#define RCC_CFGR_I2SSRC(source) (source << 23)
#define RCC_CFGR_MCO1(source) (source << 21)
#define RCC_CFGR_RTCPRE(value) (value << 16)
#define RCC_CFGR_RTCPRE_MASK (0x1F << 16)
#define RCC_CFGR_PPRE2(value) (value << 13)
#define RCC_CFGR_PPRE2_MASK (0x7 << 13)
#define RCC_CFGR_PPRE1(value) (value << 10)
#define RCC_CFGR_PPRE1_MASK (0x7 << 10)
#define RCC_CFGR_HPRE(value) (value << 4)
#define RCC_CFGR_HPRE_MASK (0xF << 4)
#define RCC_CFGR_SWS(source) (source << 2)
#define RCC_CFGR_SW(source) (source << 0)

typedef enum
{
    RCC_RTCPRE_NO_CLOCK = 0x0, // 00000-00001: No clock
    RCC_RTCPRE_DIV2 = 0x2,     // 00010: HSE/2
    RCC_RTCPRE_DIV3 = 0x3,     // 00011: HSE/3
    RCC_RTCPRE_DIV4 = 0x4,     // 00100: HSE/4
    RCC_RTCPRE_DIV5 = 0x5,     // 00101: HSE/5
    RCC_RTCPRE_DIV6 = 0x6,     // 00110: HSE/6
    RCC_RTCPRE_DIV7 = 0x7,     // 00111: HSE/7
    RCC_RTCPRE_DIV8 = 0x8,     // 01000: HSE/8
    RCC_RTCPRE_DIV9 = 0x9,     // 01001: HSE/9
    RCC_RTCPRE_DIV10 = 0xA,    // 01010: HSE/10
    RCC_RTCPRE_DIV11 = 0xB,    // 01011: HSE/11
    RCC_RTCPRE_DIV12 = 0xC,    // 01100: HSE/12
    RCC_RTCPRE_DIV13 = 0xD,    // 01101: HSE/13
    RCC_RTCPRE_DIV14 = 0xE,    // 01110: HSE/14
    RCC_RTCPRE_DIV15 = 0xF,    // 01111: HSE/15
    RCC_RTCPRE_DIV16 = 0x10,   // 10000: HSE/16
    RCC_RTCPRE_DIV17 = 0x11,   // 10001: HSE/17
    RCC_RTCPRE_DIV18 = 0x12,   // 10010: HSE/18
    RCC_RTCPRE_DIV19 = 0x13,   // 10011: HSE/19
    RCC_RTCPRE_DIV20 = 0x14,   // 10100: HSE/20
    RCC_RTCPRE_DIV21 = 0x15,   // 10101: HSE/21
    RCC_RTCPRE_DIV22 = 0x16,   // 10110: HSE/22
    RCC_RTCPRE_DIV23 = 0x17,   // 10111: HSE/23
    RCC_RTCPRE_DIV24 = 0x18,   // 11000: HSE/24
    RCC_RTCPRE_DIV25 = 0x19,   // 11001: HSE/25
    RCC_RTCPRE_DIV26 = 0x1A,   // 11010: HSE/26
    RCC_RTCPRE_DIV27 = 0x1B,   // 11011: HSE/27
    RCC_RTCPRE_DIV28 = 0x1C,   // 11100: HSE/28
    RCC_RTCPRE_DIV29 = 0x1D,   // 11101: HSE/29
    RCC_RTCPRE_DIV30 = 0x1E,   // 11110: HSE/30
    RCC_RTCPRE_DIV31 = 0x1F    // 11111: HSE/31
} RCC_RTC_Presc;

typedef enum
{
    RCC_NOT_DIV = 0x0,
    RCC_APB_DIV2 = 0x4,
    RCC_APB_DIV4 = 0x5,
    RCC_APB_DIV8 = 0x6,
    RCC_APB_DIV16 = 0x7,
} RCC_APB_Presc;

typedef enum
{
    RCC_HPRE_NOT_DIV = 0x0,
    RCC_HPRE_DIV2 = 0x8,
    RCC_HPRE_DIV4 = 0x9,
    RCC_HPRE_DIV8 = 0xA,
    RCC_HPRE_DIV16 = 0xB,
    RCC_HPRE_DIV64 = 0xC,
    RCC_HPRE_DIV128 = 0xD,
    RCC_HPRE_DIV256 = 0xE,
    RCC_HPRE_DIV512 = 0xF
} RCC_AHB_Presc;

// APB2ENR
#define RCC_APB2ENR_TIM11EN (0x1 << 18)
#define RCC_APB2ENR_TIM10EN (0x1 << 17)
#define RCC_APB2ENR_TIM9EN (0x1 << 16)
#define RCC_APB2ENR_SYSCFGEN (0x1 << 14)
#define RCC_APB2ENR_SPI1EN (0x1 << 12)
#define RCC_APB2ENR_SDIOEN (0x1 << 11)
#define RCC_APB2ENR_ADC3EN (0x1 << 10)
#define RCC_APB2ENR_ADC2EN (0x1 << 9)
#define RCC_APB2ENR_ADC1EN (0x1 << 8)
#define RCC_APB2ENR_USART6EN (0x1 << 5)
#define RCC_APB2ENR_USART1EN (0x1 << 4)
#define RCC_APB2ENR_TIM8EN (0x1 << 1)
#define RCC_APB2ENR_TIM1EN (0x1 << 0)

// APB1ENR

#define RCC_APB1ENR_DACEN (0x1 << 29)
#define RCC_APB1ENR_PWREN (0x1 << 28)
#define RCC_APB1ENR_CAN2EN (0x1 << 26)
#define RCC_APB1ENR_CAN1EN (0x1 << 25)
#define RCC_APB1ENR_I2C3EN (0x1 << 23)
#define RCC_APB1ENR_I2C2EN (0x1 << 22)
#define RCC_APB1ENR_I2C1EN (0b1 << 21)
#define RCC_APB1ENR_UART5EN (0x1 << 20)
#define RCC_APB1ENR_UART4EN (0x1 << 19)
#define RCC_APB1ENR_USART3EN (0x1 << 18)
#define RCC_APB1ENR_USART2EN (0x1 << 17)
#define RCC_APB1ENR_SPI3EN (0x1 << 15)
#define RCC_APB1ENR_SPI2EN (0x1 << 14)
#define RCC_APB1ENR_WWDGEN (0x1 << 11)
#define RCC_APB1ENR_TIM14EN (0x1 << 8)
#define RCC_APB1ENR_TIM13EN (0x1 << 7)
#define RCC_APB1ENR_TIM12EN (0x1 << 6)
#define RCC_APB1ENR_TIM7EN (0x1 << 5)
#define RCC_APB1ENR_TIM6EN (0x1 << 4)
#define RCC_APB1ENR_TIM5EN (0x1 << 3)
#define RCC_APB1ENR_TIM4EN (0x1 << 2)
#define RCC_APB1ENR_TIM3EN (0x1 << 1)
#define RCC_APB1ENR_TIM2EN (0x1 << 0)

// AHB1ENR
#define RCC_AHB1ENR_OTGHSULPIEN (0x1 << 30)
#define RCC_AHB1ENR_OTGHSEN (0x1 << 29)
#define RCC_AHB1ENR_ETHMACPTPEN (0x1 << 28)
#define RCC_AHB1ENR_ETHMACRXEN (0x1 << 27)
#define RCC_AHB1ENR_ETHMACTXEN (0x1 << 26)
#define RCC_AHB1ENR_ETHMACEN (0x1 << 25)
#define RCC_AHB1ENR_DMA2EN (0x1 << 22)
#define RCC_AHB1ENR_DMA1EN (0x1 << 21)
#define RCC_AHB1ENR_CCMDATARAMEN (0x1 << 20)
#define RCC_AHB1ENR_BKPSRAMEN (0x1 << 18)
#define RCC_AHB1ENR_CRCEN (0x1 << 12)
#define RCC_AHB1ENR_GPIOIEN (0x1 << 8)
#define RCC_AHB1ENR_GPIOHEN (0x1 << 7)
#define RCC_AHB1ENR_GPIOGEN (0x1 << 6)
#define RCC_AHB1ENR_GPIOFEN (0x1 << 5)
#define RCC_AHB1ENR_GPIOEEN (0x1 << 4)
#define RCC_AHB1ENR_GPIODEN (0x1 << 3)
#define RCC_AHB1ENR_GPIOCEN (0x1 << 2)
#define RCC_AHB1ENR_GPIOBEN (0x1 << 1)
#define RCC_AHB1ENR_GPIOAEN (0x1 << 0)

// AHB2ENR
#define RCC_AHB2ENR_OTGFSEN (0x1 << 7)
#define RCC_AHB2ENR_RNGEN (0x1 << 6)
#define RCC_AHB2ENR_HASHEN (0x1 << 5)
#define RCC_AHB2ENR_CRYPEN (0x1 << 4)
#define RCC_AHB2ENR_DCMIEN (0x1 << 0)

// AHB3ENR
#define RCC_AHB3ENR_FSMCEN (0x1 << 0)

typedef enum {
    RCC_BUS_AHB1,
    RCC_BUS_AHB2,
    RCC_BUS_AHB3,
    RCC_BUS_APB1,
    RCC_BUS_APB2
} RCC_Bus;

typedef enum
{
    RCC_HSI = 0x0,
    RCC_HSE = 0x1,
    RCC_PLL = 0x2
} RCC_ClockSource;

typedef struct
{
} RCC_PLL_InitTypeDef;

typedef struct
{
    uint32_t PLLM; // Division factor for input clock (2..63)
    uint32_t PLLN; // Multiplication factor for VCO (64..432)
    uint32_t PLLP; // Main system clock division factor (2,4,6,8)
    uint32_t PLLQ; // USB/SDIO clock division factor (optional)
    uint8_t source;
} RCC_PLLConfig;

typedef struct
{
    uint8_t ahb_prescaler;
    uint8_t apb1_prescaler;
    uint8_t apb2_prescaler;
    uint8_t rtc_prescaler;
    uint8_t mco1_prescaler;
    uint8_t mco2_prescaler;
} RCC_BusConfig;

typedef struct
{
    uint32_t APB1;
    uint32_t APB2;
    uint32_t AHB1;
    uint32_t AHB2;
    uint32_t AHB3;
} RCC_PeripheralClockConfig;

typedef struct
{
    RCC_ClockSource source;
    RCC_PLLConfig *pll_config;
    RCC_BusConfig *bus_config;
} RCC_SystemConfig;

typedef struct
{
    uint32_t SYSCLK_Freq; // Частота ядра (HCLK)
    uint32_t HCLK_Freq;   // Частота AHB шины (равна SYSCLK, если нет деления)
    uint32_t APB1_Freq;   // Частота низкоскоростной периферии (≤42 MHz для F4)
    uint32_t APB2_Freq;   // Частота высокоскоростной периферии (≤84 MHz для F4)
} RCC_Frequencies;

typedef enum
{
    RTC_SRC_NONE = 0x0,
    RTC_SRC_LSE  = 0x1,
    RTC_SRC_LSI  = 0x2,
    RTC_SRC_HSE  = 0x3 
} RCC_RTCClockSource;

// BDCR
#define RCC_BDCR_BDRST (0x1 << 16)
#define RCC_BDCR_RTCEN (0x1 << 15)
#define RCC_BDCR_RTCSEL(val) (val << 8)
#define RCC_BDCR_RTCSEL_MASK RCC_BDCR_RTCSEL(0x3) 
#define RCC_BDCR_LSEBYP (0x1 << 2)
#define RCC_BDCR_LSERDY (0x1 << 1)
#define RCC_BDCR_LSEON (0x1 << 0)

// CSR

#define RCC_CSR_LPWRRSTF (0x1 << 31)
#define RCC_CSR_WWDGRSTF (0x1 << 30)
#define RCC_CSR_IWDGRSTF (0x1 << 29)
#define RCC_CSR_SFTRSTF (0x1 << 28)
#define RCC_CSR_PORRSTF (0x1 << 27)
#define RCC_CSR_PINRSTF (0x1 << 26)
#define RCC_CSR_BORRSTF (0x1 << 25)
#define RCC_CSR_RMVF (0x1 << 24)
#define RCC_CSR_LSIRDY (0x1 << 1)
#define RCC_CSR_LSION (0x1 << 0)



extern RCC_Type *RCC;

int setup_system_config_rcc(RCC_SystemConfig *config);
void enable_gpio_clock_rcc(RCC_PeripheralClockConfig *config);
void get_clock_frequencies(RCC_Frequencies *freq);
void enable_and_reset_rcc(RCC_Bus bus, uint32_t periph_mask);
void get_rcc_clock_dividers(RCC_BusConfig *config);
uint32_t get_pll48clk_freq();
int configure_rtc_clock_rcc(uint32_t rtcsel);
uint32_t get_rtc_clock_freq();