
#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t TR; // shadow register
    uint32_t DR; // shadow register
    uint32_t CR;
    uint32_t ISR;
    uint32_t PRER;
    uint32_t WUTR;
    uint32_t CALIBR;
    uint32_t ALRMAR;
    uint32_t ALRMBR;
    uint32_t WPR;
    uint32_t SSR;
    uint32_t SHIFTR;
    uint32_t TSTR;
    uint32_t TSSSR;
    uint32_t CALR;
    uint32_t TAFCR;
    uint32_t ALRMASSR;
    uint32_t ALRMBSSR;
    uint32_t BKPxR[20];
} RTC_Type;

#define RTC_CR_COE (0x1 << 23)
#define RTC_CR_OSEL(output) (output << 21)
#define RTC_CR_POL (0x1 << 20)
#define RTC_CR_COSEL (0x1 << 19)
#define RTC_CR_BKP (0x1 << 18)   
#define RTC_CR_SUB1H (0x1 << 17) 
#define RTC_CR_ADD1H (0x1 << 16) 
#define RTC_CR_TSIE (0x1 << 15)
#define RTC_CR_WUTIE (0x1 << 14)
#define RTC_CR_ALRBIE (0x1 << 13)
#define RTC_CR_ALRAIE (0x1 << 12) 
#define RTC_CR_TSE (0x1 << 11)
#define RTC_CR_WUTE (0x1 << 10)
#define RTC_CR_ALRBE (0x1 << 9)
#define RTC_CR_ALARE (0x1 << 8) 
#define RTC_CR_DCE (0x1 << 7)
#define RTC_CR_FMT_24H (0x1 << 6)
#define RTC_CR_FMT_AM (0x1 << 6)
#define RTC_CR_BYPSHAD (0x1 << 5)
#define RTC_CR_REFCKON (0x1 << 4)
#define RTC_CR_TSEDGE (0x1 << 3)

// ISR

// r - read only
// rw - read and write
// w - write only
// rc_w0 - read, write 0 reset
// rc_w1 - read, write 1 reset
// rc_r - read, automatic reset
// rs - read, set bit
// t - switch bit

#define RTC_ISR_RECALPF (0x1 << 16)
#define RTC_ISR_TAMP2F (0x1 << 14)
#define RTC_ISR_TAMP1F (0x1 << 13)
#define RTC_ISR_TSOF (0x1 << 12)
#define RTC_ISR_TSF (0x1 << 11)
#define RTC_ISR_WUTF (0x1 << 10)
#define RTC_ISR_ALRBF (0x1 << 9)
#define RTC_ISR_ALRAF (0x1 << 8)
#define RTC_ISR_INIT (0x1 << 7)
#define RTC_ISR_INITF (0x1 << 6)
#define RTC_ISR_RSF (0x1 << 5)
#define RTC_ISR_INITS (0x1 << 4)
#define RTC_ISR_SHPF (0x1 << 3)
#define RTC_ISR_WUTWF (0x1 << 2)
#define RTC_ISR_ALRBWF (0x1 << 1)
#define RTC_ISR_alrawf (0x1 << 0)

// PREP

#define RTC_PREDIV_A(value) (value << 16)
#define RTC_PREDIV_S(value) (value << 0)

// TR and DR
#define GET_RTS_BITFIELD(value,pos, mask) ((((mask << pos) & value) >> pos))
#define GET_RTS_value(value, pos_tens, mask_tens, pos_units, mask_units) (GET_RTS_BITFIELD(value, pos_tens, mask_tens) * 10 + GET_RTS_BITFIELD(value, pos_units, mask_units)) 
#define SET_RTS_value(value, pos_tens, mask_tens, pos_units, mask_units) ((((uint32_t)(value / 10) & mask_tens) << pos_tens) | ((uint32_t)((value % 10) & mask_units) << pos_units))

#define BSD_START_YEAR 2000
#define BSD_MAX_YEAR 99
// TR
#define RTC_TR_PM (0x1 << 22)
#define RTC_TR_HT_Pos (20)
#define RTC_TR_HU_Pos (16)
#define RTC_TR_MT_Pos (12)
#define RTC_TR_MU_Pos (8)
#define RTC_TR_ST_Pos (4)
#define RTC_TR_SU_Pos (0)


#define SET_RTS_HOUR(hour) SET_RTS_value(hour, RTC_TR_HT_Pos, 0x3,  RTC_TR_HU_Pos, 0xF)
#define SET_RTS_MINUTE(min) SET_RTS_value(min, RTC_TR_MT_Pos, 0x7, RTC_TR_MU_Pos, 0xF)
#define SET_RTS_SECOND(sec) SET_RTS_value(sec, RTC_TR_ST_Pos, 0x7, RTC_TR_SU_Pos, 0xF)

#define GET_RTS_HOUR(value) GET_RTS_value(value, RTC_TR_HT_Pos, 0x3, RTC_TR_HU_Pos, 0xF)
#define GET_RTS_MINUTE(value) GET_RTS_value(value, RTC_TR_MT_Pos, 0x7, RTC_TR_MU_Pos, 0xF)
#define GET_RTS_SECOND(value) GET_RTS_value(value, RTC_TR_ST_Pos, 0x7, RTC_TR_SU_Pos, 0xF)

// DR

#define RTC_DR_YT_Pos (20)
#define RTC_DR_YU_Pos (16)
#define RTC_DR_MT_Pos (12)
#define RTC_DR_MU_Pos (8)
#define RTC_DR_DT_Pos (4)
#define RTC_DR_DU_Pos (0)

#define SET_RTS_YEAR(value) SET_RTS_value(value, RTC_DR_YT_Pos, 0xF, RTC_DR_YU_Pos, 0xF)
#define SET_RTS_WDU(value) ((uint32_t)value << 13)
#define SET_RTS_MONTH(value) SET_RTS_value(value, RTC_DR_MT_Pos, 0x1, RTC_DR_MU_Pos, 0xF)
#define SET_RTS_DAY(value) SET_RTS_value(value, RTC_DR_DT_Pos, 0x3, RTC_DR_DU_Pos, 0xF)


#define GET_RTS_YEAR(value) GET_RTS_value(value, RTC_DR_YT_Pos, 0xF, RTC_DR_YU_Pos, 0xF)
#define GET_RTS_WDU(value) ((uint32_t)value >> 13)
#define GET_RTS_MONTH(value) GET_RTS_value(value, RTC_DR_MT_Pos, 0x1, RTC_DR_MU_Pos, 0xF)
#define GET_RTS_DAY(value) GET_RTS_value(value, RTC_DR_DT_Pos, 0x3, RTC_DR_DU_Pos, 0xF)




typedef enum
{
    FORBIDEN = 0x00,
    MONDAY = 0x001,
    TUESDAY = 0x010,
    WEDNESDAY = 0x011,
    THURSDAY = 0x100,
    FRIDAY = 0x101,
    SATURDAY = 0x110,
    SUNDAY = 0x111,
} DayOfWeek;

typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
} RTC_Time_Type;

#pragma pack(push, 1)
typedef struct
{
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t week;
} RTC_Date_Type;

typedef struct
{
    RTC_Date_Type date;
    RTC_Time_Type time;
} RTC_DateTime_Type;

#pragma pack(pop)



int init_rtc();
int set_time_rtc(RTC_Time_Type *date);
int set_date_rtc(RTC_Date_Type *date);
int set_datetime_rtc(RTC_DateTime_Type *datetime);

int get_datetime_rtc(RTC_DateTime_Type *datetime);
int get_time_rtc(RTC_Time_Type *time);
int get_date_rtc(RTC_Date_Type *date);