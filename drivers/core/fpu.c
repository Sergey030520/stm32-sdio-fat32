#include "fpu.h"
#include "memory_map.h"

#define SCB_CPACR (*((volatile uint32_t *)(SCB_REG + 0x88)))

void init_fpu()
{
    SCB_CPACR |= (0xF << 20);
}