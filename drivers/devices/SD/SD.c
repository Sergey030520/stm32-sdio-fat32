#include "SD.h"
#include "log.h"
#include "led.h"
#include "timer.h"
#include "dma.h"

#define WAIT_TIME_response 1000

SD_Data_Type SD;
CID_Type sd_cid = {0};
uint32_t csd_data[4] = {0};

int CmdNoRespError()
{
    uint32_t timeout = 1000000;
    while (!(SDIO->STA & SDIO_STA_CMDSENT))
    {
        if (--timeout == 0)
        {
            return ERROR_SENT_CMD;
        }
    }
    clear_flags_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
    return 0;
}

int CmdResp1Error(CardStatus_Type *cs_pattern)
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
        return ERROR_WAIT_TIME;
    }
    if (status & SDIO_STA_CCRCFAIL)
    {
        clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
        return ERROR_CRC;
    }
    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);

    CardStatus_Type *response = (CardStatus_Type *)&SDIO->RESP[0];
    SD.state = response->CURRENT_STATE;
    cs_pattern->READY_FOR_DATA = response->READY_FOR_DATA;
    cs_pattern->APP_CMD = response->APP_CMD;

    if (response->AKE_SEQ_ERROR)
    {
        return ERROR_SEQ_AKE;
    }
    if (response->ILLEGAL_COMMAND)
    {
        return ERROR_ILLEGAL_CMD;
    }
    if (response->ADDRESS_MISALIGN)
    {
        return ERROR_ADDRESS;
    }
    if (response->CARD_ECC_DISABLED)
    {
        return ERROR_CARD_ECC;
    }
    if (response->WP_ERASE_SKIP)
    {
        return ERROR_WP_ERASE_SKIP;
    }
    if (response->CSD_OVERWRITE)
    {
        return ERROR_CSD_OVERWRITE;
    }
    if (response->ERROR)
    {
        return ERROR_UNKNOWN;
    }
    if (response->CC_ERROR)
    {
        return ERROR_CARD_CONTROLLER;
    }
    if (response->CARD_ECC_FAILED)
    {
        return ERROR_CARD_ECC_FAILED;
    }
    if (response->COM_CRC_ERROR)
    {
        return ERROR_CRC;
    }
    if (response->LOCK_UNLOCK_FAILED)
    {
        return ERROR_LOCK_UNLOCK;
    }
    if (response->WP_VIOLATION)
    {
        return ERROR_WP_VIOLATION;
    }
    if (response->ERASE_PARAM)
    {
        return ERROR_ERASE_PARAM;
    }
    if (response->ERASE_SEQ_ERROR)
    {
        return ERROR_ERASE_SEQ;
    }
    if (response->BLOCK_LEN_ERROR)
    {
        return ERROR_BLOCK_LEN;
    }
    if (response->ADDRESS_OUT_OF_RANGE)
    {
        return ERROR_OUT_OF_RANGE;
    }
    if (response->CARD_IS_LOCKED)
    {
        return ERROR_CARD_LOCKED;
    }
    if (response->ERASE_RESET)
    {
        return ERROR_RESET;
    }
    return 0;
}

int CmdResp1bError(CardStatus_Type *cs)
{
    int status = CmdResp1Error(cs);
    if (status == 0)
    {
        return status;
    }
    if (status != ERROR_WAIT_TIME && status < 0)
    {
        return status;
    }

    return wait_card_ready();
}

int CmdResp2Error(uint8_t type)
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_ICR_CTIMEOUTC);
        return ERROR_WAIT_TIME;
    }
    if (status & SDIO_STA_CCRCFAIL)
    {
        clear_flag_sdio(SDIO_ICR_CCRCFAILC);
        return ERROR_CRC;
    }

    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);

    if (type == SDIO_TYPE_R2_CSD)
    {
        sdio_read_long_resp(csd_data, sizeof(csd_data));
    }
    else
    {
        sdio_read_long_resp((uint32_t *)&sd_cid, sizeof(sd_cid));
    }

    return 0;
}

// R3 -  CRC no check
int CmdResp3Error()
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    clear_flag_sdio(SDIO_ICR_CCRCFAILC);
    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_ICR_CTIMEOUTC);
        return ERROR_WAIT_TIME;
    }

    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);

    OCR_Type *response = (OCR_Type *)&SDIO->RESP[0];

    if (response->busy == 0)
    {
        return 1;
    }
    SD.state = READY_STATE;

    if (response->CCS)
    {
        SD.version = CARD_SDXC;
    }
    else
    {
        SD.version = CARD_SDSC;
    }
    SD.voltage = response->voltage;

    return 0;
}

int CmdResp4Error()
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_ICR_CTIMEOUTC);
        return ERROR_WAIT_TIME;
    }
    if (status & SDIO_STA_CCRCFAIL)
    {
        clear_flag_sdio(SDIO_ICR_CCRCFAILC);
        return ERROR_CRC;
    }
    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
    return 0;
}
int CmdResp6Error()
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_ICR_CTIMEOUTC);
        return ERROR_WAIT_TIME;
    }
    if (status & SDIO_STA_CCRCFAIL)
    {
        clear_flag_sdio(SDIO_ICR_CCRCFAILC);
        return ERROR_CRC;
    }
    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);

    Resp6_Format *response = (Resp6_Format *)&SDIO->RESP[0];
    SD.state = response->current_state;

    if (response->AKE_SEQ_ERROR)
    {
        return ERROR_SEQ_AKE;
    }
    if (response->com_crc_error)
    {
        return ERROR_CRC;
    }
    if (response->error)
    {
        return ERROR_UNKNOWN;
    }
    if (response->illegal_command)
    {
        return ERROR_ILLEGAL_CMD;
    }

    SD.RCA = response->RCA;
    return 0;
}
int CmdResp7Error()
{
    uint32_t status = 0;
    do
    {
        status = SDIO->STA;
    } while (!(status & (SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)));

    if (status & SDIO_STA_CTIMEOUT)
    {
        clear_flag_sdio(SDIO_ICR_CTIMEOUTC);
        return ERROR_WAIT_TIME;
    }
    if (status & SDIO_STA_CCRCFAIL)
    {
        clear_flag_sdio(SDIO_ICR_CCRCFAILC);
        return ERROR_CRC;
    }
    clear_flag_sdio(SDIO_MASK_STATIC_CMD_FLAGS);

    uint32_t response = SDIO->RESP[0];
    if (!(response & SD_VHC_CHECK_PATERN))
    {
        return ERROR_MASK_VHC;
    }
    return 0;
}

int reset_sd()
{
    clear_flags_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
    send_command_sdio(CMD_GO_IDLE_STATE, 0, SDIO_CMD_NO_RESP); // GO_IDLE_STATE
    int status = CmdNoRespError();
    clear_flags_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
    if (status)
    {
        LOG_INFO("Command wasn't sent! Cmd: %d", CMD_GO_IDLE_STATE);
        return ERROR_SENT_DATA;
    }
    SD.state = IDLE_STATE;
    SD.version = CARD_V_UNKNOWN;
    SD.RCA = 0;
    return 0;
}

int validateOperatingVoltage()
{
    send_command_sdio(CMD_SEND_IF_COND, SD_VHC_CHECK_PATERN, SDIO_CMD_SHORT_RESP);
    return CmdResp7Error();
}

int send_command_specific(uint32_t app_cmd, uint32_t arg, uint32_t format_resp)
{
    int status = 0;
    uint8_t attempts = 0;
    CardStatus_Type cs_pattern;
    while (1)
    {
        // APP_CMD
        send_command_sdio(CMD_APP_CMD, ((uint32_t)SD.RCA << 16), SDIO_CMD_SHORT_RESP);

        status = CmdResp1Error(&cs_pattern);

        if (status < 0 && cs_pattern.APP_CMD != 1)
        {
            return status;
        }
        else if (status == 0)
        {
            break;
        }
        else if (++attempts == 8)
        {
            return -3;
        }
        LOG_INFO(" status 55: %d", status);
    }
    send_command_sdio(app_cmd, arg, format_resp);
    return 0;
}

int set_inactive_state()
{
    send_command_sdio(CMD_GO_INACTIVE_STATE, (SD.RCA << 16), SDIO_CMD_NO_RESP);
    if (CmdNoRespError())
    {
        LOG_INFO("Command wasn't sent! Cmd: %d", CMD_GO_INACTIVE_STATE);
        return ERROR_SENT_DATA;
    }
    reset_sd();
    return 0;
}

int get_status()
{

    return 0;
}

int init_sd()
{
    int status = 0;
    SD.RCA = 0;

    status = init_sdio();

    if (status != 0)
    {
        ledOn(13, 1);
        LOG_INFO("Error init sdio! Code: %d", status);
        return -1;
    }

    status = reset_sd();

    if (status)
    {
        LOG_INFO("Error reset sd! Code: %d", status);
        return -1;
    }

    status = validateOperatingVoltage();

    if (status == 0)
    {
        SD.version = CARD_V2_X;
        LOG_INFO("supported VHS!");
    }
    else if (status == ERROR_WAIT_TIME)
    {
        LOG_INFO("unsupported VHS!");
        SD.version = CARD_V1_X;

        status = reset_sd();

        if (status)
        {
            LOG_INFO("Error reset sd! Code: %d", status);
            return -1;
        }
    }
    else
    {
        LOG_INFO("Error  sd! Code: %d", status);
        return -2;
    }

    int count = 0;
    while ((++count <= SD_MAX_VOLT_TRIAL) && (SD.voltage == 0))
    {
        // CMD55 + ACMD41
        status = send_command_specific(ACMD_SEND_OP_COND,
                                       SD_HIGH_CAPACITY | SD_WINDOW_VOLTAGE,
                                       SDIO_CMD_SHORT_RESP);
        if (status)
        {
            LOG_INFO("ACMD41 send fail, status=%d", status);
            return -1;
        }

        // Проверка ответа R3
        status = CmdResp3Error();
        if (status < 0)
        {
            LOG_INFO("ACMD41 response error: %d", status);
            set_inactive_state();
            return -1;
        }

        if (SD.voltage == 0)
        {
            delay_timer(10); // 10 мс между попытками
        }
    }

    if (count > SD_MAX_VOLT_TRIAL)
        return -1;

    LOG_INFO("SD {capacity type %d, voltage: %x}", SD.version, SD.voltage);

    send_command_sdio(CMD_ALL_SEND_CID, 0, SDIO_CMD_LONG_RESP);
    status = CmdResp2Error(SDIO_TYPE_R2_CID);
    if (status != 0)
    {
        LOG_INFO("Error Cmd: %d Code: %d", CMD_ALL_SEND_CID, status);
        set_inactive_state();
        return -1;
    }

    char product_name[5];
    for (int i = 0; i < 4; ++i)
        product_name[i] = sd_cid.PNM[i];
    product_name[4] = '\0';
    LOG_INFO("MID %u, OID %u, product name: %s, product version: %u, serial number: %lu, MDT: %x, CRC: %u", sd_cid.manufacture_id,
             sd_cid.OEM, product_name, sd_cid.PRV, sd_cid.PSN, sd_cid.MDT,
             sd_cid.CRC);

    send_command_sdio(CMD_SEND_RELATIVE_ADDR, 0, SDIO_CMD_SHORT_RESP);
    status = CmdResp6Error();
    if (status != 0)
    {
        LOG_INFO("Error Cmd: %d Code: %d", CMD_SEND_RELATIVE_ADDR, status);
        set_inactive_state();
        return -1;
    }
    LOG_INFO("Set new RCA: %u; state: %u", SD.RCA, SD.state);
    return 0;
}

uint32_t math_pow(uint32_t value, uint32_t exp)
{
    uint32_t result = 1;
    if (exp <= 1)
        return (exp ? value : 1);

    while (exp)
    {
        if (exp & 1)
            result *= value;
        exp >>= 1;
        value *= value;
    }
    return result;
}

int read_csd_data(CSD_Type *csd)
{
    if (csd == NULL)
    {
        return -1;
    }
    csd->CSD_STRUCTURE = (uint8_t)((csd_data[0] & 0xc0000000U) >> 30);
    csd->TAAC = (uint8_t)((csd_data[0] & 0x00FF0000U) >> 16);
    csd->NSAC = (uint8_t)((csd_data[0] & 0x0000FF00U) >> 8);
    csd->TRAN_SPEED = (uint8_t)(csd_data[0] & 0x000000FFU);

    csd->CCC = (uint8_t)((csd_data[1] & 0xfff00000U) >> 20);
    csd->READ_BL_LEN = (uint8_t)((csd_data[1] & 0xf0000U) >> 16);
    csd->READ_BL_PARTIAL = (uint8_t)((csd_data[1] & 0x8000U) >> 15);
    csd->WRITE_BLK_MISALIGN = (uint8_t)((csd_data[1] & 0x4000U) >> 14);
    csd->READ_BLK_MISALIGN = (uint8_t)((csd_data[1] & 0x2000U) >> 13);
    csd->DSR_IMP = (uint8_t)((csd_data[1] & 0x1000U) >> 12);

    if (csd->CSD_STRUCTURE == CSD_VERSION_1)
    {
        csd->C_SIZE = (uint32_t)(((csd_data[1] & 0x000003FFU) << 2U) | ((csd_data[2] & 0xC0000000U) >> 30U));
        csd->VDD_R_CURR_MIN = (uint8_t)((csd_data[0] & 0x38000000U) >> 27);
        csd->VDD_R_CURR_MAX = (uint8_t)((csd_data[0] & 0x07000000U) >> 24);
        csd->VDD_W_CURR_MIN = (uint8_t)((csd_data[0] & 0x00E00000U) >> 21);
        csd->VDD_W_CURR_MAX = (uint8_t)((csd_data[0] & 0x001C0000U) >> 18);
        csd->C_SIZE_MULT = (uint8_t)((csd_data[0] & 0x00038000U) >> 15);
    }
    else if (csd->CSD_STRUCTURE == CSD_VERSION_2)
    {
        csd->C_SIZE = (uint32_t)(((csd_data[1] & 0x0000003FU) << 16U) | ((csd_data[2] & 0xFFFF0000U) >> 16U));
    }
    else
    {
        return -2;
    }

    csd->ERASE_BLK_EN = (uint8_t)((csd_data[2] & 0x4000U) >> 14);
    csd->SECTOR_SIZE = (uint8_t)((csd_data[2] & 0x3f80U) >> 7);
    csd->WP_GRP_SIZE = (uint8_t)(csd_data[2] & 0x007fU);

    csd->WP_GRP_ENABLE = (uint8_t)((csd_data[3] & 0x80000000U) >> 31);
    csd->R2W_FACTOR = (uint8_t)((csd_data[3] & 0x1c000000U) >> 26);
    csd->WRITE_BL_LEN = (uint8_t)((csd_data[3] & 0x3c00000U) >> 22);
    csd->WRITE_BL_PARTIAL = (uint8_t)((csd_data[3] & 0x200000U) >> 21);

    csd->FILE_FORMAT_GRP = (uint8_t)((csd_data[3] & 0x00008000U) >> 15);
    csd->COPY = (uint8_t)((csd_data[3] & 0x00004000U) >> 14);
    csd->PERM_WRITE_PROTECT = (uint8_t)((csd_data[3] & 0x00002000U) >> 13);
    csd->TMP_WRITE_PROTECT = (uint8_t)((csd_data[3] & 0x00001000U) >> 12);
    csd->FILE_FORMAT = (uint8_t)((csd_data[3] & 0x00000C00U) >> 10);
    csd->CRC = (uint8_t)((csd_data[3] & 0x000000FEU) >> 1);
    return 0;
}

int load_csd_sd()
{
    int status = 0;
    send_command_sdio(CMD_SEND_CSD, ((uint32_t)SD.RCA << 16), SDIO_CMD_LONG_RESP);
    status = CmdResp2Error(SDIO_TYPE_R2_CSD);
    if (status < 0)
    {
        LOG_INFO("Error sent cmd: %d status: %d", CMD_SEND_CSD, status);
        return -1;
    }

    CSD_Type csd = {0};

    if (read_csd_data(&csd) != 0)
    {
        return -2;
    }
    LOG_INFO("CSD version: %d, class command: %x, DSR: %d,", (csd.CSD_STRUCTURE + 1), csd.CCC, csd.DSR_IMP);

    if (csd.CSD_STRUCTURE == CSD_VERSION_1)
    {
        uint32_t block_len = math_pow(2, (uint32_t)csd.READ_BL_LEN);
        uint32_t mult = math_pow(2, (uint32_t)csd.C_SIZE_MULT);
        SD.max_size = ((csd.C_SIZE + 1) * mult) * block_len;

        LOG_INFO("VDD_W_CURR_MIN: %x, VDD_R_CURR_MIN: %x,", csd.VDD_W_CURR_MIN, csd.VDD_R_CURR_MIN);
        LOG_INFO("VDD_W_CURR_MIN: %x, VDD_R_CURR_MIN: %x,", csd.VDD_W_CURR_MAX, csd.VDD_R_CURR_MAX);
        LOG_INFO("WRITE_BL_LEN: %x, READ_BL_LEN: %x", csd.WRITE_BL_LEN, csd.READ_BL_LEN);
        LOG_INFO("C_SIZE_MULT: %d, C_SIZE: %d", csd.C_SIZE_MULT, csd.C_SIZE);
        LOG_INFO("SD capacity: %d", SD.max_size);
        LOG_INFO("COPY: %x, file format: %x,", csd.COPY, csd.FILE_FORMAT);
    }
    else if (csd.CSD_STRUCTURE == CSD_VERSION_2)
    {
        SD.max_size = (csd.C_SIZE + 1) * (512 * 1024);
        LOG_INFO("WRITE_BL_LEN: %d, READ_BL_LEN: %x", csd.WRITE_BL_LEN, csd.READ_BL_LEN);
        LOG_INFO("SD capacity: %d", SD.max_size);
        LOG_INFO("C_SIZE: %d, ", csd.C_SIZE);
        LOG_INFO("COPY: %x, file format: %x,", csd.COPY, csd.FILE_FORMAT);
    }
    else
    {
        LOG_INFO("Stnadart CSD version > 2 unsupported!");
        return WARNING_CSD_UNSUPPORTED_VERSION;
    }

    return 0;
}

void show_state()
{
    switch (SD.state)
    {
    case IDLE_STATE:
        LOG_INFO("SD state: IDLE");
        break;
    case READY_STATE:
        LOG_INFO("SD state: READY");
        break;
    case IDENT_STATE:
        LOG_INFO("SD state: IDENT");
        break;

    case STBY_STATE:
        LOG_INFO("SD state: STBY");
        break;

    case TRAN_STATE:
        LOG_INFO("SD state: TRAN");
        break;

    case DATA_STATE:
        LOG_INFO("SD state: DATA");
        break;

    case RCV_STATE:
        LOG_INFO("SD state: RCV");
        break;

    case PRG_STATE:
        LOG_INFO("SD state: PRG");
        break;

    case DIS_STATE:
        LOG_INFO("SD state: DIS");
        break;

    case INA_STATE:
        LOG_INFO("SD state: INA");
        break;
    }
}

int send_status_sd(CardStatus_Type *cs_pattern)
{
    int status = 0;
    if (cs_pattern == NULL)
        return -1;

    send_command_sdio(CMD_SEND_STATUS, ((uint32_t)SD.RCA << 16), SDIO_CMD_SHORT_RESP);

    status = CmdResp1Error(cs_pattern);

    if (status == ERROR_WAIT_TIME || status == ERROR_CRC)
    {
        LOG_INFO("Error sent cmd: %d, error: %d;", CMD_SEND_STATUS, status);
        return ERROR_RESPONCE;
    }
    return status;
}

int wait_card_ready()
{
    uint16_t attempts = 500; 
    int status = 0;
    CardStatus_Type card_status = {0};

    while (attempts--)
    {
        status = send_status_sd(&card_status); 
        if (status == ERROR_RESPONCE)
        {
            return status; 
        }

        
        if (status == 0 && card_status.READY_FOR_DATA == 1 && SD.state == TRAN_STATE)
        {
            return 0; 
        }

        delay_timer(10);
    }

    return ERROR_SD_BUSY_TIMEOUT; 
}

int init_sd_card()
{
    int status = 0;
    if (load_csd_sd() < 0)
    {
        return -1;
    }

    send_command_sdio(CMD_SELECT_CARD, ((uint32_t)SD.RCA << 16), SDIO_CMD_SHORT_RESP);

    CardStatus_Type cs_pattern = {0};
    status = CmdResp1bError(&cs_pattern);
    if (status == ERROR_SD_BUSY_TIMEOUT)
    {
        LOG_INFO("Error: SD busy timeout");
        return -1;
    }
    if (status < 0)
    {
        LOG_INFO("Error recv cmd: %d code: %d", CMD_SELECT_CARD, status);
        return -2;
    }
    LOG_INFO("Card [rsa: %d] select!", SD.RCA);

    // устанавливаем 4 битную шину передачи
    status = send_command_specific(ACMD_SET_BUS_WIDTH, BIT4_WIDTH_SD, SDIO_CMD_SHORT_RESP);
    if (status < 0)
    {
        LOG_INFO("Command wasn't sent! Cmd: %d Code: %d", ACMD_SET_BUS_WIDTH, status);
    }

    memset((uint8_t *)&cs_pattern, 0, sizeof(CardStatus_Type));
    cs_pattern.APP_CMD = 0x1;
    status = CmdResp1Error(&cs_pattern);
    if (status < 0)
    {
        LOG_INFO("Error Cmd: %d Code: %d", ACMD_SET_BUS_WIDTH, status);
        set_inactive_state();
        return -3;
    }

    set_bus_width_sdio(WIDE_BUS_4BIT_MODE);
    LOG_INFO("Set BUS width %d", 4);

    // настраиваем частотут тактирования
    set_clock_frequency_sdio(2);

    return 0;
}

int write_single_block_sd(uint8_t *data, uint32_t size, uint32_t addres_block, uint32_t timeout)
{
    int status = 0;
    Card_Status cs_pattern = {0};

    if (size != 512)
    {
        return -10;
    }

    SDIO->DCTRL = 0;

    sdio_config_data(SDIO_DCTRL_DBLOCKSIZE(BLOCK_SIZE_512B) | SDIO_DCTRL_DTEN, 512, timeout);

    // set address write
    send_command_sdio(CMD_WRITE_BLOCK, addres_block, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error((CardStatus_Type *)&cs_pattern);
    if (status < 0)
    {
        LOG_INFO("Error set address block for write [Code error: %d]", status);
        return -1;
    }

    // uint32_t *ptrBuffTx = (uint32_t *)data;

    uint32_t *temp_buff = (uint32_t *)data;
    int dataremaining = size;

    while (!(SDIO->STA & (SDIO_STA_TXUNDERR | SDIO_STA_STBITERR | SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL | SDIO_STA_DATAEND)))
    {
        // отправляем 8 пакетов проверяем статусы на ошибку
        if ((SDIO->STA & SDIO_STA_TXFIFOHE) && dataremaining > 0)
        {
            for (int i = 0; i < 8; ++i)
            {
                SDIO->FIFO = *temp_buff++;
                dataremaining -= 4;
            }
        }
    }

    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_DATA_CRC;
    }

    if (SDIO->STA & SDIO_STA_TXUNDERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_TXUNDER;
    }
    if (SDIO->STA & SDIO_STA_DTIMEOUT)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
    return wait_card_ready();
}

int write_multi_block_sd(uint8_t *data, uint32_t count_blocks, uint32_t address, uint32_t timeout)
{
    int status = 0;
    CardStatus_Type cs = {0};
    uint32_t length = 512 * count_blocks;

    if (count_blocks <= 1)
    {
        if (count_blocks == 1)
            return write_single_block_sd(data, length, address, timeout);
        else
            return 0;
    }

    if (SD.version != CARD_SDXC)
    {
        address *= 512;
    }

    SDIO->DCTRL = 0;

    sdio_config_data(SDIO_DCTRL_DBLOCKSIZE(BLOCK_SIZE_512B) | SDIO_DCTRL_DTEN, length, timeout);

    send_command_sdio(CMD_WRITE_MULTIPLE_BLOCK, address, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
        LOG_INFO("Error set address for CMD_WRITE_MULTIPLE_BLOCK [Code error: %d]", status);
        return -1;
    }

    uint32_t *temp_buff = (uint32_t *)data;
    int dataremaining = length;

    while (!(SDIO->STA & (SDIO_STA_TXUNDERR | SDIO_STA_STBITERR | SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL | SDIO_STA_DATAEND)))
    {
        // отправляем 8 пакетов проверяем статусы на ошибку
        if ((SDIO->STA & SDIO_STA_TXFIFOHE) && (dataremaining > 0))
        {
            for (int i = 0; i < 8; ++i)
            {

                SDIO->FIFO = *temp_buff++;
                dataremaining -= 4;
            }
        }
    }
    if (SDIO->STA & SDIO_STA_DATAEND)
    {
        send_command_sdio(CMD_STOP_TRANSMISSION, 0, SDIO_CMD_SHORT_RESP);
        status = CmdResp1bError(&cs);

        if (status != 0)
        {
            clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
            LOG_INFO("Error stop transmission [Code error: %d]", status);
            return -1;
        }
    }

    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_DATA_CRC;
    }

    if (SDIO->STA & SDIO_STA_TXUNDERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_TXUNDER;
    }
    if (SDIO->STA & SDIO_STA_DTIMEOUT)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
    return wait_card_ready();
}

int write_blocks_dma(uint8_t *data, uint32_t count_blocks, uint32_t address, uint32_t timeout)
{
    int status = 0;
    CardStatus_Type cs = {0};
    uint32_t length = 512 * count_blocks;

    if (count_blocks <= 1)
    {
        if (count_blocks == 1)
            return write_single_block_sd(data, length, address, timeout);
        else
            return 0;
    }

    if (SD.version != CARD_SDXC)
    {
        address *= 512;
    }

    SDIO->DCTRL = 0;

    sdio_config_data(SDIO_DCTRL_DBLOCKSIZE(BLOCK_SIZE_512B) | SDIO_DCTRL_DTEN, length, timeout);

    send_command_sdio(CMD_WRITE_MULTIPLE_BLOCK, address, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_CMD_FLAGS);
        LOG_INFO("Error set address for CMD_WRITE_MULTIPLE_BLOCK [Code error: %d]", status);
        return -1;
    }

    SDIO->DCTRL |= SDIO_DCTRL_DMAEN;

    DMA_Stream stream = STREAM_3;

    // запускаем dma
    set_enabled_dma(DMA2, stream, 0);

    DMA2->streams[stream].CCR = DMA_CHSEL(CHANNEL_4) | DMA_MEM_TO_PERIPH | DMA_MINC | DMA_PSIZE_32BITS | DMA_MSIZE_32BITS | DMA_PL_HIGH;
    DMA2->streams[stream].CNDTR = (count_blocks * 512 / sizeof(SDIO->FIFO));
    DMA2->streams[stream].CPAR = (uint32_t)&SDIO->FIFO;
    DMA2->streams[stream].CMAR0 = (uint32_t)&data;
    DMA2->streams[stream].FCR = DMA_FCR_DMDIS | DMA_FCR_FTH(FTH_3_4_FIFO);

    set_enabled_dma(DMA2, stream, 1);

    while (!(SDIO->STA & (SDIO_STA_TXUNDERR | SDIO_STA_STBITERR | SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL | SDIO_STA_DATAEND)))
        ;
    while (!(DMA2->LISR & (TCIFx(stream) | TEIFx(stream) | FEIFx(stream) | DMEIFx(stream))))
        ;

    if (SDIO->STA & SDIO_STA_DATAEND)
    {
        send_command_sdio(CMD_STOP_TRANSMISSION, 0, SDIO_CMD_SHORT_RESP);
        status = CmdResp1bError(&cs);

        if (status != 0)
        {
            clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
            reset_flags_dma(DMA2, stream);
            LOG_INFO("Error stop transmission [Code error: %d]", status);
            return -1;
        }
    }

    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        reset_flags_dma(DMA2, stream);
        LOG_INFO("Data CRC error during transmission");
        return ERROR_DATA_CRC;
    }

    if (SDIO->STA & SDIO_STA_TXUNDERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        reset_flags_dma(DMA2, stream);
        LOG_INFO("Transmit underflow error");
        return ERROR_TXUNDER;
    }

    if (SDIO->STA & SDIO_STA_DTIMEOUT)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        reset_flags_dma(DMA2, stream);
        LOG_INFO("Transmission timeout");
        return ERROR_WAIT_TIME;
    }

    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
    reset_flags_dma(DMA2, stream);

    return 0;
}

int read_single_block_sd(uint8_t *buffer, uint32_t size_buff, uint32_t address, uint32_t timeout)
{
    int status = 0;
    Card_Status cs_pattern = {0};

    if (size_buff != 512)
    {
        return -1;
    }

    SDIO->DCTRL = 0;

    if (SD.version != CARD_SDXC)
    {
        address *= 512U;
    }

    send_command_sdio(CMD_READ_SINGLE_BLOCK, address, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error((CardStatus_Type *)&cs_pattern);
    if (status < 0)
    {
        LOG_INFO("Error set address block [error: %d]!", status);
        return -1;
    }

    sdio_config_data(SDIO_DCTRL_DBLOCKSIZE(BLOCK_SIZE_512B) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN, 512, timeout);

    int idx = 0;
    while (!(SDIO->STA & (SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL | SDIO_STA_STBITERR | SDIO_STA_RXOVERR | SDIO_STA_DBCKEND)))
    {
        if (SDIO->STA & SDIO_STA_RXFIFOF)
        {
            *((uint32_t *)(buffer + idx)) = SDIO->FIFO;
            idx += sizeof(uint32_t);
        }
    }

    if (SDIO->STA & SDIO_STA_DTIMEOUT)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    if (SDIO->STA & SDIO_STA_RXOVERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_RXOVERR;
    }

    if (SDIO->STA & SDIO_STA_STBITERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_START_BIT;
    }

    while (SDIO->STA & SDIO_STA_RXDAVL)
    {
        *((uint32_t *)(buffer + idx)) = SDIO->FIFO;
        idx += sizeof(uint32_t);
    }
    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
    return wait_card_ready();
}

int read_multi_block_sd(uint8_t *buffer, uint32_t count_blocks, uint32_t address, uint32_t timeout)
{
    int status = 0;
    uint32_t length = count_blocks * 512;
    CardStatus_Type cs = {0};

    if (count_blocks <= 1)
    {
        if (count_blocks == 1)
            return read_single_block_sd(buffer, length, address, timeout);
        else
            return 0;
    }

    if (SD.version != CARD_SDXC)
    {
        address *= 512;
    }

    SDIO->DCTRL = 0;

    sdio_config_data(SDIO_DCTRL_DBLOCKSIZE(BLOCK_SIZE_512B) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN, length, timeout);

    send_command_sdio(CMD_READ_MULTIPLE_BLOCK, address, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        LOG_INFO("Error set address for CMD_READ_MULTIPLE_BLOCK [Error code: %d]", status);
        return -1;
    }

    uint32_t data_remaining = length;
    uint32_t *temp_buff = (uint32_t *)buffer;

    while (!(SDIO->STA & (SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL | SDIO_STA_STBITERR | SDIO_STA_RXOVERR | SDIO_STA_DATAEND)))
    {
        if ((SDIO->STA & SDIO_STA_RXFIFOHF) && (data_remaining > 0))
        {

            for (int i = 0; i < 8; ++i)
            {
                *temp_buff = SDIO->FIFO;
                ++temp_buff;
                data_remaining -= sizeof(SDIO->FIFO);
            }
        }
    }
    if (SDIO->STA & SDIO_STA_DATAEND)
    {
        send_command_sdio(CMD_STOP_TRANSMISSION, 0, SDIO_CMD_SHORT_RESP);
        status = CmdResp1bError(&cs);
        if (status != 0)
        {
            clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
            LOG_INFO("Error stop transmission [Error code %d]", status);
            return -1;
        }
    }

    if (SDIO->STA & SDIO_STA_DTIMEOUT)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    if (SDIO->STA & SDIO_STA_DCRCFAIL)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_WAIT_TIME;
    }

    if (SDIO->STA & SDIO_STA_RXOVERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_RXOVERR;
    }

    if (SDIO->STA & SDIO_STA_STBITERR)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        return ERROR_START_BIT;
    }

    while ((SDIO->STA & SDIO_STA_RXDAVL) && data_remaining > 0)
    {
        *temp_buff = SDIO->FIFO;
        ++temp_buff;
        data_remaining -= sizeof(SDIO->FIFO);
    }

    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);

    delay_timer(100);
    return wait_card_ready();
}

int read_blocks_dma(uint8_t *buffer, uint32_t count_blocks, uint32_t address, uint32_t timeout)
{
    return 0;
}



int erase_sd(uint32_t address_start, uint32_t address_end)
{
    int status = 0;
    CardStatus_Type cs = {0};

    if (address_start > address_end)
    {
        uint32_t save = address_start;
        address_start = address_end;
        address_end = save;
    }

    SDIO->DCTRL = 0;

    CSD_Type csd;

    if (read_csd_data(&csd) != 0)
    {
        return -1;
    }

    if (!(csd.CCC & CARD_CLASS_5))
    {
        LOG_INFO("Error class unsupported [%x]", csd.CCC);
        return 1;
    }

    if (SD.version == CARD_SDSC)
    {
        address_start *= 512;
        address_end *= 512;
    }

    status = wait_card_ready();
    if (status != 0)
    {
        LOG_INFO("Card not ready before erase start [Code: %d]", status);
        return status;
    }

    send_command_sdio(CMD_ERASE_WR_BLK_START, address_start, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        LOG_INFO("Error set erase start address [%d] [Code error: %d]", address_start, status);
        return -1;
    }

    send_command_sdio(CMD_ERASE_WR_BLK_END, address_end, SDIO_CMD_SHORT_RESP);
    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        LOG_INFO("Error set erase end address [Code error: %d]", status);
        return -1;
    }

    send_command_sdio(CMD_ERASE, 0U, SDIO_CMD_SHORT_RESP);

    status = CmdResp1Error(&cs);
    if (status != 0)
    {
        clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);
        LOG_INFO("Error set erase end address [Code error: %d]", status);
        return -1;
    }
    clear_flags_sdio(SDIO_MASK_STATIC_DATA_FLAGS);

    delay_timer(100);
    return wait_card_ready();
    ;
}
