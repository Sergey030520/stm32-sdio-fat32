#include "log.h"
#include <stdio.h>
#include <stdarg.h>


LogLevel current_level = LEVEL_DEBUG;

static DataTransferFunc transfer_func = NULL;

void stm_init_log(DataTransferFunc func){
    transfer_func = func;
}

void stm_log(LogLevel level, uint16_t err_code, const char *file, int line, char *format, ...)
{
    if (level < current_level)
        return;

    char buff[4096];
    va_list args;

    uint16_t size = 0;
    int offset = snprintf(buff, sizeof(buff), "[%s:%d] ", file, line);

    if (offset < 0 || offset >= sizeof(buff))
        return;

    va_start(args, format);
    size = vsnprintf(buff + offset, sizeof(buff) - offset, format, args);
    va_end(args);

    size += offset;

#ifdef LOG_CMD_MODE
    LogMessage message = {
        .buffer = buff,
        .length = size,
        .level = (uint8_t)level};


    transfer_func(message, sizeof(message));
#else
    transfer_func(buff, size);
#endif
}

void stm_log_hex(LogLevel level, uint16_t err_code, const char *file, int line, uint8_t *data, int size, LogGroupBytes group)
{
    if (size <= 0 || size > 4000)
        return;

    char buffer[4000];
    char *ptr = buffer;
    char *end = buffer + sizeof(buffer) - 1;


    for (int byte = 0; byte < size; byte += group)
    {
        if (ptr + 1 >= end)  
            break;

        switch (group)
        {
        case GROUP_1BYTE:
            ptr += snprintf(ptr, end - ptr, "%02X ", data[byte]);
            break;
        case GROUP_2BYTE:
            if (byte + 1 < size)
                ptr += snprintf(ptr, end - ptr, "%02X%02X ", data[byte], data[byte + 1]);
            else
                ptr += snprintf(ptr, end - ptr, "%02X ", data[byte]);  // Последний байт без пары
            break;
        case GROUP_4BYTE:
            if (byte + 3 < size)
                ptr += snprintf(ptr, end - ptr, "%02X%02X%02X%02X ", data[byte], data[byte + 1], data[byte + 2], data[byte + 3]);
            else
            {
                for (int j = byte; j < size; j++)
                    ptr += snprintf(ptr, end - ptr, "%02X ", data[j]);
            }
            break;
        default:
            break;
        }
    }
    *ptr = '\0';

    stm_log(level, err_code, file, line, "%s", buffer);
}