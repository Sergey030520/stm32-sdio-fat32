#pragma once

#include <stdint.h>


// POWER
#define SDIO_PWRCTRL_ON (0x3 << 0)
#define SDIO_PWRCTRL_OFF (0x0 << 0)

// CLKCR
#define SDIO_CLKCR_HWFC_EN (0x1 << 14) // HW Flow Control enable
#define SDIO_CLKCR_NEGEDGE (0x1 << 13) // dephasing selection bit
#define SDIO_CLKCR_WIDBUS1 (0x00 << 11)
#define SDIO_CLKCR_WIDBUS4 (0x01 << 11)
#define SDIO_CLKCR_WIDBUS8 (0x10 << 11)
#define SDIO_CLKCR_WIDBUS(wide_bus) (wide_bus << 11)
#define SDIO_CLKCR_BYPASS (0x1 << 10)
#define SDIO_CLKCR_PWRSAV (0x1 << 9)
#define SDIO_CLKCR_CLKEN (0x1 << 8)

#define SDIO_CLKCR_CLKDIV(value) (value << 0) // 208 - 400kgz

#define SDIO_CLKR_CLEAR_MASK (~0xFFFFFFFF)
#define SDIO_MASK_CLKDIV (0xFF << 0)
#define SDIO_MASK_BUSWIDTH (0x3 << 11)
// CMD
#define SDIO_CMD_ATACMD ((uint32_t)0x1 << 14)      // CE-ATA command
#define SDIO_CMD_nIEN ((uint32_t)0x1 << 13)        // if this bit is 0, interrupts in the CE-ATA device are enabled
#define SDIO_CMD_ENCMDcompl ((uint32_t)0x1 << 12)  // Используется для работы с прерываниями после завершения команды.
#define SDIO_CMD_SDIOSuspend ((uint32_t)0x1 << 11) // Указывает, является ли команда командой приостановки
#define SDIO_CMD_CPSMEN ((uint32_t)0x1 << 10)      // Включает передачу команды
#define SDIO_CMD_WAITPEND ((uint32_t)0x1 << 9)     // Ожидание передачи: 1 – ожидать завершение передачи данных перед отправкой команды
#define SDIO_CMD_WAITINT ((uint32_t)0x1 << 8)      // Ожидание прерывания: 1 – ожидать прерывание, 0 – не ожидать.
#define SDIO_CMD_NO_RESP ((uint32_t)0b00 << 6)
#define SDIO_CMD_SHORT_RESP ((uint32_t)0b01 << 6)
#define SDIO_CMD_LONG_RESP ((uint32_t)0b11 << 6)

// DCTRL
#define SDIO_DCTRL_SDIOEN (0x1 << 11) // Если работа идет с SDIO-устройством
#define SDIO_DCTRL_RW_D2 (0x0 << 10)  // Read wait mode
#define SDIO_DCTRL_RW_CK (0x1 << 10)  // Read wait mode
#define SDIO_DCTRL_RWSTOP (0x1 << 9)  // Read wait stop
#define SDIO_DCTRL_RWSTART (0x1 << 8)
#define SDIO_DCTRL_DBLOCKSIZE(size) ((uint32_t)size << 4)
#define SDIO_DCTRL_DMAEN (0x1 << 3)
#define SDIO_DCTRL_DTMODE (0x1 << 2) // 0 - block data transfer, tream or SDIO multibyte data transfer
#define SDIO_DCTRL_DTDIR (0x1 << 1)  // 0 - From controller to card, 1 - from card to controller
#define SDIO_DCTRL_DTEN (0x1 << 0)   // Data transfer enabled bit

// ICR
#define SDIO_STA_CEATAEND (0x1 << 23)
#define SDIO_STA_SDIOIT (0x1 << 22)
#define SDIO_STA_RXDAVL (0x1 << 21)
#define SDIO_STA_TXDAVL (0x1 << 20)
#define SDIO_STA_RXFIFOE (0x1 << 19)
#define SDIO_STA_TXFIFOE (0x1 << 18)
#define SDIO_STA_RXFIFOF (0x1 << 17)
#define SDIO_STA_TXFIFOF (0x1 << 16)
#define SDIO_STA_RXFIFOHF (0x1 << 15)
#define SDIO_STA_TXFIFOHE (0x1 << 14)
#define SDIO_STA_RXACT (0x1 << 13)
#define SDIO_STA_TXACT (0x1 << 12)
#define SDIO_STA_CMDACT (0x1 << 11)
#define SDIO_STA_DBCKEND (0x1 << 10)
#define SDIO_STA_STBITERR (0x1 << 9)
#define SDIO_STA_DATAEND (0x1 << 8)
#define SDIO_STA_CMDSENT (0x1 << 7)
#define SDIO_STA_CMDREND (0x1 << 6)
#define SDIO_STA_RXOVERR (0x1 << 5)
#define SDIO_STA_TXUNDERR (0x1 << 4)
#define SDIO_STA_DTIMEOUT (0x1 << 3)
#define SDIO_STA_CTIMEOUT (0x1 << 2)
#define SDIO_STA_DCRCFAIL (0x1 << 1)
#define SDIO_STA_CCRCFAIL (0x1 << 0)

// ICR

#define SDIO_ICR_CEATAENDC SDIO_STA_CEATAEND
#define SDIO_ICR_SDIOITC SDIO_STA_SDIOIT
#define SDIO_ICR_DBCKENDC SDIO_STA_DBCKEND
#define SDIO_ICR_STBITERRC SDIO_STA_STBITERR
#define SDIO_ICR_DATAENDC SDIO_STA_DATAEND
#define SDIO_ICR_CMDSENTC SDIO_STA_CMDSENT
#define SDIO_ICR_CMDRENDC SDIO_STA_CMDREND
#define SDIO_ICR_RXOVERRC SDIO_STA_RXOVERR
#define SDIO_ICR_TXUNDERRC SDIO_STA_TXUNDERR
#define SDIO_ICR_DTIMEOUTC SDIO_STA_DTIMEOUT
#define SDIO_ICR_CTIMEOUTC SDIO_STA_CTIMEOUT
#define SDIO_ICR_DCRCFAILC SDIO_STA_DCRCFAIL
#define SDIO_ICR_CCRCFAILC SDIO_STA_CCRCFAIL

#define SDIO_MASK_STATIC_DATA_FLAGS ((uint32_t)(SDIO_ICR_DCRCFAILC | SDIO_ICR_DTIMEOUTC | SDIO_ICR_TXUNDERRC | \
                                                SDIO_ICR_RXOVERRC | SDIO_ICR_DATAENDC | SDIO_ICR_DBCKENDC))
#define SDIO_MASK_STATIC_CMD_FLAGS ((uint32_t)(SDIO_ICR_CCRCFAILC | SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | \
                                               SDIO_STA_CMDSENT))

// MASK

#define SDIO_MASK_CEATAENDIE SDIO_STA_CEATAEND
#define SDIO_MASK_SDIOITIE SDIO_STA_SDIOIT
#define SDIO_MASK_RXDAVLIE SDIO_STA_RXDAVL
#define SDIO_MASK_TXDAVLIE SDIO_STA_TXDAVL
#define SDIO_MASK_RXFIFOEIE SDIO_STA_RXFIFOE
#define SDIO_MASK_TXFIFOEIE SDIO_STA_TXFIFOE
#define SDIO_MASK_RXFIFOFIE SDIO_STA_RXFIFOF
#define SDIO_MASK_TXFIFOFIE SDIO_STA_TXFIFOF
#define SDIO_MASK_RXFIFOHFIE SDIO_STA_RXFIFOHF
#define SDIO_MASK_TXFIFOHEIE SDIO_STA_TXFIFOHE
#define SDIO_MASK_RXACTIE SDIO_STA_RXACT
#define SDIO_MASK_TXACTIE SDIO_STA_TXACT
#define SDIO_MASK_CMDACTIE SDIO_STA_CMDACT
#define SDIO_MASK_DBCKENDIE SDIO_STA_DBCKEND
#define SDIO_MASK_STBITERRIE SDIO_STA_STBITERR
#define SDIO_MASK_DATAENDIE SDIO_STA_DATAEND
#define SDIO_MASK_CMDSENTIE SDIO_STA_CMDSENT
#define SDIO_MASK_CMDRENDIE SDIO_STA_CMDREND
#define SDIO_MASK_RXOVERRIE SDIO_STA_RXOVERR
#define SDIO_MASK_TXUNDERRIE SDIO_STA_TXUNDERR
#define SDIO_MASK_DTIMEOUTIE SDIO_STA_DTIMEOUT
#define SDIO_MASK_CTIMEOUTIE SDIO_STA_CTIMEOUT
#define SDIO_MASK_DCRCFAILIE SDIO_STA_DCRCFAIL
#define SDIO_MASK_CCRCFAILIE SDIO_STA_CCRCFAIL

//

#define SDIO_MASK_ERROR 0x33

typedef struct
{
    uint32_t POWER;
    uint32_t CLKCR;
    uint32_t ARG;
    uint32_t CMD;
    uint32_t RESPCMD;
    uint32_t RESP[4]; // CARD status 4
    uint32_t DTIMER;
    uint32_t DLEN;
    uint32_t DCTRL;
    uint32_t DCOUNT;
    uint32_t STA;
    uint32_t ICR;
    uint32_t MASK;
    uint32_t RESERVED0[2];
    uint32_t FIFOCNT;
    uint32_t RESERVED1[13];
    uint32_t FIFO;
} SDIO_Type;

extern SDIO_Type *SDIO;

typedef enum
{
    POWER_OFF,
    POWER_ON
} SDIO_Power_State;

typedef struct
{
    uint32_t idx_cmd;
    uint32_t args;
    uint32_t param;
    uint32_t dctrl;
    uint32_t timer;
} SDIO_CMD_Type;

typedef enum
{
    BLOCK_SIZE_BYTE = 0x0,
    BLOCK_SIZE_2B = 0x1,
    BLOCK_SIZE_4B = 0x2,
    BLOCK_SIZE_8B = 0x3,
    BLOCK_SIZE_16B = 0x4,
    BLOCK_SIZE_32B = 0x5,
    BLOCK_SIZE_64B = 0x6,
    BLOCK_SIZE_128B = 0x7,
    BLOCK_SIZE_256B = 0x8,
    BLOCK_SIZE_512B = 0x9,
    BLOCK_SIZE_1024B = 0xA,
    BLOCK_SIZE_2048B = 0xB,
    BLOCK_SIZE_4096B = 0xC,
    BLOCK_SIZE_8192B = 0xD,
    BLOCK_SIZE_16384B = 0xE
} SDIO_BLOCK_SIZE_DATA;

typedef enum
{
    WIDE_BUS_1BIT_MODE = 0x00,
    WIDE_BUS_4BIT_MODE = 0x01,
    WIDE_BUS_8BIT_MODE = 0x10,
} WIDE_BUS_SDIO;

typedef void (*sdio_callback_t)(void);

int configure_power_SDIO(SDIO_Power_State state);
// void send_command_sdio(SDIO_CMD_Type *command);
void send_command_sdio(uint32_t command, uint32_t arg, uint32_t format_resp);
void sdio_read_long_resp(uint32_t *data, uint16_t size);
void sdio_config_data(uint32_t data_control, uint32_t length, uint32_t timeout);

int init_sdio();
void set_clock_frequency_sdio(uint8_t div);
void set_bus_width_sdio(WIDE_BUS_SDIO wide_bus);
void clear_flags_sdio(uint32_t mask);
void clear_flag_sdio(uint32_t flag);

// IRQ
void set_irq_sdio(uint32_t mask, sdio_callback_t handler);
void reset_irq_sdio(uint32_t mask);
void SDIO_IRQHandler();
