#pragma once


#include <stdint.h>


typedef struct{
    uint32_t CR;
    uint32_t CSR; 
}Power_Type;


#define POWER_CR_VOS (0x1 << 14)
#define POWER_CR_FPDS (0x1 << 9)
#define POWER_CR_DBP (0x1 << 8)
#define POWER_CR_PLS(voltage) (voltage << 5)
#define POWER_CR_PVDE (0x1 << 4)
#define POWER_CR_CSBF (0x1 << 3)
#define POWER_CR_CWUF (0x1 << 2)
#define POWER_CR_PDDS (0x1 << 1)
#define POWER_CR_LPDS (0x1 << 0)

extern Power_Type *PWR;