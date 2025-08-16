#include "IRQ.h"
#include "memory_map.h"


NVIC_Type *NVIC = NVIC_REG;


static inline void __enable_irq(void)
{
    asm volatile("cpsie i" : : : "memory");
}

void init_irq(){
     NVIC->ISERx[1] = NVIC_SDIO; //| NVIC_USART1;
    // NVIC->ICERx[1] = NVIC_DMA2_STREAM4;
    NVIC->ICERx[2] = NVIC_DMA2_STREAM7;
    __enable_irq();
}