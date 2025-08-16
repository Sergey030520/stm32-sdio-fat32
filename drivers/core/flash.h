#pragma once 


#include <stdint.h>

// flash
// #define CPU_CYCLES 6U
// #define LATENCY (CPU_CYCLES << 0)

#define FLASH_ACR (FLASH_REG << 0)
#define FLASH_MASK_LATENCY (0x7 << 0)
#define FLASH_SET_LATENCY(cpu_cycles) ((cpu_cycles & 0x7) << 0)


#define FLASH_DCRST (0x1 << 11) // Data cache reset
#define FLASH_ICRST (0x1 << 11) // Instruction cache reset
#define FLASH_DCEN (0x1 << 10)  // Data cache enable
#define FLASH_ICEN (0x1 << 9)   // Instruction cache enable
#define FLASH_PRFTEN (0x1 << 8) // Prefetch enable

typedef enum {
    FLASH_LATENCY_0 = 0x0,
    FLASH_LATENCY_1 = 0x1,
    FLASH_LATENCY_2 = 0x2,
    FLASH_LATENCY_3 = 0x3,
    FLASH_LATENCY_4 = 0x4,
    FLASH_LATENCY_5 = 0x5,
    FLASH_LATENCY_6 = 0x6,
    FLASH_LATENCY_7 = 0x7
} FLASH_Latency;

typedef struct
{
    uint32_t ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
    uint32_t SR;
    uint32_t CR;
    uint32_t OPTCR;
} FLASH_Type;
