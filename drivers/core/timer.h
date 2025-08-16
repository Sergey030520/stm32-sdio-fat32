#include <stdint.h>

#pragma once

typedef struct
{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SMCR;
    uint32_t DIER;
    uint32_t SR;
    uint32_t EGR;
    uint32_t CCMR1;
    uint32_t CCMR2;
    uint32_t CCER;
    uint32_t CNT;
    uint32_t PSC;
    uint32_t ARR;
    uint32_t reserved;
    uint32_t CCR1;
    uint32_t CCR2;
    uint32_t CCR3;
    uint32_t CCR4;
    uint32_t reserved2;
    uint32_t DCR;
    uint32_t DMAR;
    uint32_t TIM2_OR;
    uint32_t TIM5_OR;
} GP_Timer_Type1;



typedef struct
{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t reserved;
    uint32_t DIER;
    uint32_t SR;
    uint32_t EGR;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t CNT;
    uint32_t PSC;
    uint32_t ARR;
} Basic_Timer_Type;

typedef struct
{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SMCR;
    uint32_t DIER;
    uint32_t SR;
    uint32_t EGR;
    uint32_t CCMR1;
    uint32_t CCMR2;
    uint32_t CCER;
    uint32_t CNT;
    uint32_t PSC;
    uint32_t ARR;
    uint32_t RCR;
} Advanced_Timer_Type;

// CR1

#define TIMER_CR1_ARPE (0x1 << 7) // auto-reload preload enable
#define TIMER_CR1_DIR_UP (0x0 << 4)
#define TIMER_CR1_DIR_DOWN (0x1 << 4)
#define TIMER_CR1_CEN (0x1 << 0)

// Interrupt
#define TIMER_DIER_UDE (0x1 << 8)
#define TIMER_DIER_UIE (0x1 << 0)

// SR
#define TIMER_SR_UIF (0x1 << 0)

// EGR
#define TIMER_EGR_UG (0x1 << 0)

// CCMRx output
#define TIMER_CCMRx_OC1M_PWM (0b110)
#define TIMER_CCMRx_OC1M_INV_PWM (0b111)

// CCER
#define TIMER_CCER_CCx(channel, polarity) ((polarity << (channel * 4 + 1)) | (0b1 << (channel * 4)))

typedef enum
{
    TIM_CHANNEL1 = 0,
    TIM_CHANNEL2 = 1,
    TIM_CHANNEL3,
    TIM_CHANNEL4
} Timer_Channel;

void delay_timer(uint16_t ms);