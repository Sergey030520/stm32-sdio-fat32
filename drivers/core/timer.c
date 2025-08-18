#include "timer.h"
#include "memory_map.h"
#include "rcc.h"

void timer_init()
{
    RCC_Frequencies rcc_clocks = {0};
    RCC_BusConfig rcc_config = {0};
    get_clock_frequencies(&rcc_clocks);
    get_rcc_clock_dividers(&rcc_config);
    GP_Timer_Type *timer = (GP_Timer_Type *)TIM2_REG;
    timer->CR1 = 0;

    uint32_t timer_clk = rcc_clocks.APB1_Freq;
    if (rcc_config.apb1_prescaler > 1)
        timer_clk *= 2;
    timer->PSC = (timer_clk / 1000) - 1; // 1 тик = 1 мс
    timer->CR1 |= TIMER_CR1_ARPE;                   // авто-перезагрузка
}

void delay_timer(uint32_t ms)
{
    GP_Timer_Type *timer = (GP_Timer_Type *)TIM2_REG;
    timer->ARR = ms - 1;
    timer->CNT = 0;
    timer->SR = 0;

    timer->CR1 |= TIMER_CR1_CEN;

    while (!(timer->SR & TIMER_SR_UIF))
        ;

    timer->SR &= ~TIMER_SR_UIF;
    timer->CR1 &= ~TIMER_CR1_CEN;
}
