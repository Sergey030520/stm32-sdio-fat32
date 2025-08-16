#pragma once


#include <stdint.h>

// NVIC

typedef struct
{
    uint32_t ICTR; // Interrupt Controller Type Register
    uint32_t RESERV1[62];
    uint32_t ISERx[8]; // Interrupt Set-Enable Registers
    uint32_t RESERV2[24];
    uint32_t ICERx[8]; // Interrupt Clear-Enable Registers
    uint32_t RESERV3[24];
    uint32_t ISPRx[8]; // Interrupt Set-Pending Registers
    uint32_t RESERV4[24];
    uint32_t ICPRx[8]; // Interrupt Clear-Pending Registers
    uint32_t RESERV5[24];
    uint32_t IABRx[8]; // Interrupt Active Bit Register
    uint32_t RESERV6[56];
    uint32_t IPRx[60]; // Interrupt Priority Register
} NVIC_Type;

#define NVIC_DMA2_STREAM4 (0x1 << 28) // 60
#define NVIC_DMA2_STREAM7 (0x1 << 6)  // 70

#define NVIC_USART1 (0x1 << 5) // 37
#define NVIC_SDIO (0x1 << 17)  // 49

extern NVIC_Type *NVIC;

void init_irq();