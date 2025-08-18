#include "RTC.h"
#include "memory_map.h"
#include "gpio.h"
#include "rcc.h"
#include "timer.h"


RTC_Type *RTC = RTC_REG;
static uint8_t is_sync_date_rtc = 0;

static int rtc_initialized = 0;

uint32_t rtc_get_prer_value();
void calculate_prescalers_rtc(uint32_t f_ck_rtc, uint32_t *prediv_a, uint32_t *prediv_s);



void rtc_wait_sync(void)
{
    RTC->ISR &= ~RTC_ISR_RSF;
    while (!(RTC->ISR & RTC_ISR_RSF));
    is_sync_date_rtc = 1;
}

int init_rtc()
{
    if(configure_rtc_clock_rcc(RTC_SRC_HSE)!= 0) return -1;

    // если память была заблокирована
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF))
        ;
    
    RTC->PRER = rtc_get_prer_value();

    RTC->ISR &= ~RTC_ISR_INIT;

    RTC->ISR &= ~RTC_ISR_RSF;
    while (!(RTC->ISR & RTC_ISR_RSF));

    rtc_wait_sync();

    RTC->WPR = 0xFF;

    rtc_initialized = 1;
    return 0;
}

int set_datetime_rtc(RTC_DateTime_Type *datetime)
{
    if (datetime == 0)
    {
        return -1;
    }
    uint8_t year = datetime->date.year - BSD_START_YEAR;
    if (year > BSD_MAX_YEAR || year < 0)
    {
        return -2;
    }

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF))
        ;

    RTC->TR = SET_RTS_HOUR(datetime->time.hour) | SET_RTS_MINUTE(datetime->time.minute) | SET_RTS_SECOND(datetime->time.seconds);
    RTC->DR = SET_RTS_YEAR(year) | SET_RTS_WDU(datetime->date.week) | SET_RTS_MONTH(datetime->date.month) | SET_RTS_DAY(datetime->date.day);

    RTC->ISR &= ~RTC_ISR_INIT;

    RTC->WPR = 0xFF;

    rtc_wait_sync();
    
    return 0;
}

int set_time_rtc(RTC_Time_Type *time)
{
    if (time == 0)
    {
        return -1;
    }

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF))
        ;

    RTC->TR = SET_RTS_HOUR(time->hour) | SET_RTS_MINUTE(time->minute) | SET_RTS_SECOND(time->seconds);

    RTC->ISR &= ~RTC_ISR_INIT;

    RTC->WPR = 0xFF;
    
    rtc_wait_sync();
    
    return 0;
}

int set_date_rtc(RTC_Date_Type *date)
{
    if (date == 0)
    {
        return -1;
    }

    if (date->year - BSD_START_YEAR > BSD_MAX_YEAR || date->year - BSD_START_YEAR < 0)
    {
        return -2;
    }

    RCC_BusConfig config = {0};
    get_rcc_clock_dividers(&config);

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF))
        ;


    RTC->DR = SET_RTS_YEAR(date->year) | SET_RTS_WDU(date->week) | SET_RTS_MONTH(date->month) | SET_RTS_DAY(date->day);

    RTC->ISR &= ~RTC_ISR_INIT;

    RTC->WPR = 0xFF;

    rtc_wait_sync();

    return 0;
}

int get_date_rts(RTC_Date_Type *date)
{
    if (!date || !rtc_initialized) return -1;
    RTC_DateTime_Type datetime;
    if (get_datetime_rtc(&datetime)) return -1;
    *date = datetime.date;
    return 0;
}

int get_time_rts(RTC_Time_Type *time)
{
    if (!time || !rtc_initialized) return -1;
    RTC_DateTime_Type datetime;
    if (get_datetime_rtc(&datetime)) return -1;
    *time = datetime.time;
    return 0;
}

int get_datetime_rtc(RTC_DateTime_Type *datetime)
{
    if (!datetime || !rtc_initialized) return -1;

    if (!is_sync_date_rtc)
        rtc_wait_sync();

    datetime->time.hour = GET_RTS_HOUR(RTC->TR);
    datetime->time.minute = GET_RTS_MINUTE(RTC->TR);
    datetime->time.seconds = GET_RTS_SECOND(RTC->TR);

    datetime->date.day = GET_RTS_DAY(RTC->DR);
    datetime->date.month = GET_RTS_MONTH(RTC->DR);
    datetime->date.week = GET_RTS_WDU(RTC->DR);
    datetime->date.year = GET_RTS_YEAR(RTC->DR) + BSD_START_YEAR;

    return 0;
}

void calculate_prescalers_rtc(uint32_t f_ck_rtc, uint32_t *prediv_a, uint32_t *prediv_s)
{
    uint32_t a = 0;
    uint32_t s = 0;
    for (a = 0; a <= 127; a++)
    {
        s = (f_ck_rtc / (a + 1)) - 1;
        if (s <= 0x7FFF) // синхронный предделитель 15 бит
        {
            *prediv_a = a;
            *prediv_s = s;
            return;
        }
    }

    // Если не найдено подходящее значение — ставим максимальные
    *prediv_a = 127;
    *prediv_s = 0x7FFF;
}

uint32_t rtc_get_prer_value()
{
    uint32_t prediv_a, prediv_s;
    calculate_prescalers_rtc(get_rtc_clock_freq(), &prediv_a, &prediv_s);
    return RTC_PREDIV_A(prediv_a) | RTC_PREDIV_S(prediv_s);
}