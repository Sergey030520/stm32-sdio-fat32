#include "timer.h"
#include "memory_map.h"
#include "rcc.h"



void delay_timer(uint16_t ms)
{
    RCC_Frequencies rcc_clocks = {0};
    RCC_BusConfig rcc_config = {0};
    get_clock_frequencies(&rcc_clocks);
    get_rcc_clock_dividers(&rcc_config);

    uint32_t timer_clk = rcc_clocks.APB1_Freq;

    if (rcc_config.apb1_prescaler > 1)
    {
        timer_clk *= 2;
    }

    GP_Timer_Type1 *timer = TIM2_REG;
    timer->CR1 = 0;
    timer->PSC = (((uint32_t)timer_clk / 1000) - 1);
    
    timer->CNT = 0; // 1 msec
    timer->ARR = ms - 1;
    timer->SR = 0; 

    timer->EGR = TIMER_EGR_UG;          
    timer->CR1 |= TIMER_CR1_CEN ;

    while (!(timer->SR & TIMER_SR_UIF))
        ;
    
    timer->SR &= ~TIMER_SR_UIF;
    timer->CR1 &= ~TIMER_CR1_CEN;
}