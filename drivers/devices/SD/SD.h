#include <stdint.h>
#include "sdio.h"

#pragma once

typedef enum
{
    IDLE_STATE = 0x0,
    READY_STATE = 0x1,
    IDENT_STATE = 0x2,
    STBY_STATE = 0x3,
    TRAN_STATE = 0x4,
    DATA_STATE = 0x5,
    RCV_STATE = 0x6,
    PRG_STATE = 0x7,
    DIS_STATE = 0x8,
    INA_STATE = 0x9
} SD_State;

//  Responce

typedef struct
{
    uint32_t reserved : 3;
    uint32_t AKE_SEQ_ERROR : 1;
    uint32_t reserved_sd : 1;
    uint32_t APP_CMD : 1;
    uint32_t reserved1 : 1;
    uint32_t SWITCH_ERROR : 1;
    uint32_t READY_FOR_DATA : 1;
    uint32_t CURRENT_STATE : 4;
    uint32_t ERASE_RESET : 1;
    uint32_t CARD_ECC_DISABLED : 1;
    uint32_t WP_ERASE_SKIP : 1;
    uint32_t CSD_OVERWRITE : 1; // or CID
    uint32_t reserved2 : 2;
    uint32_t ERROR : 1;
    uint32_t CC_ERROR : 1;
    uint32_t CARD_ECC_FAILED : 1;
    uint32_t ILLEGAL_COMMAND : 1;
    uint32_t COM_CRC_ERROR : 1;
    uint32_t LOCK_UNLOCK_FAILED : 1;
    uint32_t CARD_IS_LOCKED : 1;
    uint32_t WP_VIOLATION : 1;
    uint32_t ERASE_PARAM : 1;
    uint32_t ERASE_SEQ_ERROR : 1;
    uint32_t BLOCK_LEN_ERROR : 1;
    uint32_t ADDRESS_MISALIGN : 1;
    uint32_t ADDRESS_OUT_OF_RANGE : 1;
} CardStatus_Type; // R1



#pragma pack(push, 1)

typedef struct
{
    uint8_t CRC; 
    uint8_t FILE_FORMAT;
    uint8_t TMP_WRITE_PROTECT;
    uint8_t PERM_WRITE_PROTECT;
    uint8_t COPY;
    uint8_t FILE_FORMAT_GRP; 
    uint8_t WRITE_BL_PARTIAL;
    uint8_t WRITE_BL_LEN; 
    uint8_t R2W_FACTOR;
    uint8_t WP_GRP_ENABLE;
    uint8_t WP_GRP_SIZE;
    uint8_t SECTOR_SIZE;
    uint8_t ERASE_BLK_EN;
    uint8_t C_SIZE_MULT;
    uint8_t VDD_R_CURR_MIN;
    uint8_t VDD_R_CURR_MAX;
    uint8_t VDD_W_CURR_MIN;
    uint8_t VDD_W_CURR_MAX;
    uint32_t C_SIZE;
    uint8_t DSR_IMP;
    uint8_t READ_BLK_MISALIGN;
    uint8_t WRITE_BLK_MISALIGN;
    uint8_t READ_BL_PARTIAL;
    uint8_t READ_BL_LEN;
    uint16_t CCC;
    uint8_t TRAN_SPEED;
    uint8_t NSAC;
    uint8_t TAAC;
    uint8_t CSD_STRUCTURE;
} CSD_Type;

#pragma pack(pop)

typedef enum{
    CSD_VERSION_1 = 0x0,
    CSD_VERSION_2 = 0x1
}CSD_Version;

typedef enum {
    CARD_CLASS_0 = 0x0,
    CARD_CLASS_1 = 0x1,
    CARD_CLASS_2 = 0x2,
    CARD_CLASS_3 = 0x3,
    CARD_CLASS_4 = 0x4,
    CARD_CLASS_5 = 0x5, //Class for cmds ERASE memory
    CARD_CLASS_6 = 0x6,
    CARD_CLASS_7 = 0x7,
    CARD_CLASS_8 = 0x8,
    CARD_CLASS_9 = 0x9,
    CARD_CLASS_10 = 0xA,
    CARD_CLASS_11 = 0xB
}CardClass;


#pragma pack(push, 1)
typedef struct
{
    uint8_t reserved[49];
    uint16_t UHS_AU_SIZE : 4;
    uint16_t UHS_SPEED_GRADE : 4;
    uint16_t ERASE_OFFSET : 2;
    uint16_t ERASE_TIMEOUT : 6;
    uint16_t ERASE_SIZE;
    uint8_t RESERV : 4;
    uint8_t AU_SIZE : 4;
    uint8_t PERFORMANCE_MOVE : 8;
    uint8_t SPEED_CLASS : 8;
    uint32_t SIZE_OF_PROTECTED_AREA;
    uint16_t SD_CARD_TYPE;
    uint16_t RESERV_SPEC3 : 13;
    uint16_t SECURED_MODE : 1;
    uint16_t DAT_BUS_WIDTH : 2;
} Card_Status; // 136 bits

typedef struct
{
    uint8_t NOT_USED : 1;
    uint8_t CRC : 7;
    uint16_t MDT : 12; // Date Type
    uint16_t reserved : 4;
    uint32_t PSN;
    int8_t PRV;
    uint8_t PNM[5];
    uint16_t OEM;
    uint8_t manufacture_id;
} CID_Type; // R2

typedef struct
{
    uint16_t month : 4;
    uint16_t year : 8;
} DATE_Type;

#pragma pack(pop)

typedef struct
{
    uint32_t reserved : 14;
    uint32_t voltage : 9;
    uint32_t reserved3 : 6;
    uint32_t CCS : 1;
    uint32_t busy : 1;
} OCR_Type; // R3

typedef struct
{
    uint32_t reserved : 3; // first byte unknown in reference manual stm
    uint32_t AKE_SEQ_ERROR : 1;
    uint32_t reserved_sd : 1;
    uint32_t app_cmd : 1;
    uint32_t reserved1 : 2;
    uint32_t ready_for_data : 1;
    uint32_t current_state : 4;
    uint32_t error : 1;
    uint32_t illegal_command : 1;
    uint32_t com_crc_error : 1;
    uint32_t RCA : 16;
} Resp6_Format;

#pragma pack(push, 1)

typedef struct
{
    uint16_t reserv[24];
    uint16_t DataStructureVersion : 8;
    uint16_t mode_group1 : 4;
    uint16_t mode_group2 : 4;
    uint16_t mode_group3 : 4;
    uint16_t mode_group4 : 4;
    uint16_t mode_group5 : 4;
    uint16_t mode_group6 : 4;
    uint16_t group1;
    uint16_t group2;
    uint16_t group3;
    uint16_t group4;
    uint16_t group5;
    uint16_t group6;
    uint16_t current_power;
} SWITCH_Status_Type;

typedef struct
{
    uint32_t reserved;
    uint16_t SUPPORT : 2;
    uint16_t reserved2 : 9;
    uint16_t EX_SECURITY : 4;
    uint16_t SD_SPEC3 : 1;
    uint16_t SD_BUS_WIDTHS : 4;
    uint16_t SD_SECURITY : 3;
    uint16_t DATA_STAT_AFTER_ERASE : 1;
    uint16_t SD_SPEC : 4;
    uint16_t SCR_STRUCTURE : 4;
} SCR_Typedef;

#pragma pack(pop)

// Commands SD
typedef enum
{
    CMD_GO_IDLE_STATE = 0b0,      // No responce
    CMD_ALL_SEND_CID = 0x2,       // R2
    CMD_SEND_RELATIVE_ADDR = 0x3, // R6
    CMD_SET_DSR = 0x4,            // No responce
    CMD_SWITCH_FUNC = 0x6,        // R1
    CMD_SELECT_CARD = 0x7,        // R1b - only from the select card [DESELECT_CARD]
    CMD_SEND_IF_COND = 0x8,       // R7
    CMD_SEND_CSD = 0x9,           // R2
    CMD_SEND_CID = 0xA,           // R2
    CMD_VOLTAGE_SWITCH = 0XB,     // R1
    CMD_STOP_TRANSMISSION = 0xC,  // R1b
    CMD_SEND_STATUS = 0xD,        // R1 [SEND_TASK_STATUS]
    CMD_GO_INACTIVE_STATE = 0xF,
    CMD_SET_BLOCKLEN = 0x10,         // R1
    CMD_READ_SINGLE_BLOCK = 0x11,    // R1
    CMD_READ_MULTIPLE_BLOCK = 0x12,  // R1
    CMD_SEND_TUNING_BLOCK = 0x13,    // R1
    CMD_SPEED_CLASS_CONTROL = 0x14,  // R1b
    CMD_ADDR_EXTENSION = 0x16,       // R1
    CMD_SET_BLOCK_COUNT = 0x17,      // R1
    CMD_WRITE_BLOCK = 0x18,          // R1
    CMD_WRITE_MULTIPLE_BLOCK = 0x19, // R1
    CMD_PROGRAM_CSD = 0x1B,          // R1
    CMD_SET_WRITE_PROT = 0x1C,       // R1b
    CMD_CLR_WRITE_PROT = 0x1D,       // R1b
    CMD_SEND_WRITE_PROT = 0x1E,      // R1
    CMD_ERASE_WR_BLK_START = 0x20,   // R1
    CMD_ERASE_WR_BLK_END = 0x21,     // R1
    CMD_ERASE = 0x26,                // R1b
    CMD_DEFINED_BY_DPS_SPEC = 0x28,  // R1
    CMD_LOCK_UNLOCK = 0x2A,          // R1
    CMD_APP_CMD = 0x37,              // R1
    CMD_GEN_CMD = 0x38               // R1
} SD_COMMAND;

typedef enum
{
    ACMD_SET_BUS_WIDTH = 0x6,           // R1
    ACMD_APP_CMD_STATUS = 0xD,          // R1
    ACMD_SEND_NUM_WR_BLOCKS = 0x16,     // R1
    ACMD_SET_WR_BLK_ERASE_COUNT = 0x17, // R1
    ACMD_SEND_OP_COND = 0x29,           // R3
    ACMD_SET_CLR_CARD_DETECT = 0x2A,    // R1
    ACMD_SEND_SCR = 0x33,               // R1
    ACMD_SECURE_RECEIVE = 0x35,         // R1
    ACMD_SECURE_SEND = 0x36             // R1
} SD_APP_COMMAND;
typedef enum {
    BIT1_WIDTH_SD = 0b00,
    BIT4_WIDTH_SD = 0b10,
}WIDTH_BUS_SD;

// ACMD

#define SDIO_TYPE_R2_CID 0
#define SDIO_TYPE_R2_CSD 1

typedef enum
{
    WARNING_DATA_UNAVAILABLE = 10,
    WARNING_CSD_SIZE = 11,
    WARNING_CSD_UNSUPPORTED_VERSION = 12
} SD_Warning;

//
typedef enum
{
    ERROR_WAIT_TIME = -1,
    ERROR_CRC = -2,
    ERROR_ERASE = -3,
    ERROR_ADDRESS = -4,
    ERROR_ARG = -5,
    ERROR_ILLEGAL_CMD = -6,
    ERROR_STATE = -7,
    ERROR_ERASE_PARAM = -8,
    ERROR_CARD_LOCKED = -9,
    ERROR_CARD_CONTROLLER = -10,
    ERROR_WP_VIOLATION = -11,
    ERROR_CARD_ECC = -13,
    ERROR_UNKNOWN = -14, // Resp2->error
    ERROR_OUT_OF_RANGE = -15,
    ERROR_WP_ERASE_SKIP = -16,
    ERROR_RESET = -17,
    ERROR_MASK_VHC = -18,
    ERROR_VOLTAGE_UNSUPPORTED = -19,
    ERROR_SENT_DATA = -20,
    ERROR_SEQ_AKE = -21,
    ERROR_APP_CMD = -22,
    ERROR_CSD_OVERWRITE = -23,
    ERROR_CARD_ECC_FAILED = -24,
    ERROR_LOCK_UNLOCK = -25,
    ERROR_ERASE_SEQ = -26,
    ERROR_BLOCK_LEN = -27,
    ERROR_SENT_CMD = -28,
    ERROR_SD_BUSY_TIMEOUT = -29,
    ERROR_RESPONCE = -30,
    ERROR_TXUNDER = -31,
    ERROR_DATA_CRC = -32,
    ERROR_STOP_TRANSMISSION = -34,
    ERROR_START_BIT = -35,
    ERROR_RXOVERR = -36
} SD_ErrorCode;

#pragma pack(push, 1)

typedef struct
{
    SD_State state;
    uint32_t voltage;
    uint8_t version;
    uint16_t RCA;
    uint64_t max_size;
} SD_Data_Type;

#define CARD_SDXC 4
#define CARD_SDSC 3
#define CARD_V2_X 2
#define CARD_V1_X 1
#define CARD_V_UNKNOWN 0

#define SD_MAX_VOLT_TRIAL 40
// Voltage
#define SD_WINDOW_VOLTAGE ((0x1 << 21) | (0x1 << 20))
#define SD_VHC_CHECK_PATERN ((uint32_t)0x000001AAU)
#define SD_HIGH_CAPACITY (0x1 << 30)
#define SD_SWITCH_1_8V_CAPACITY (0x01000000U)



#pragma pack(pop)

// Check responce
int CmdNoRespError();
int CmdResp1Error();
int CmdResp1bError();
int CmdResp2Error(uint8_t type);
int CmdResp3Error();
int CmdResp6Error();
int CmdResp7Error();

void show_state();


int send_status_sd();
int wait_card_ready();
int read_csd_data(CSD_Type *csd);
int validateOperatingVoltage();
int send_command_specific(uint32_t app_cmd, uint32_t arg, uint32_t format_resp);
int set_inactive_state();

int init_sd();
int init_sd_card();

// Write data
int write_single_block_sd(uint8_t *data, uint32_t size, uint32_t addres_block, uint32_t timeout_date);
int write_multi_block_sd(uint8_t *data, uint32_t count_blocks, uint32_t address, uint32_t timeout);
int write_blocks_dma(uint8_t *data, uint32_t count_blocks, uint32_t address, uint32_t timeout);

// Read data
int read_single_block_sd(uint8_t *buffer, uint32_t size_buff, uint32_t address, uint32_t timeout);
int read_multi_block_sd(uint8_t *buffer, uint32_t count_blocks, uint32_t address, uint32_t timeout);
int read_blocks_dma(uint8_t *buffer, uint32_t count_blocks, uint32_t address, uint32_t timeout);

// Erase data
int erase_sd(uint32_t address_start, uint32_t address_end);