#include "log_stm.h"
#include "log.h"




void fat32_stm_log(Fat32LogLevel level, const char *file, int line, const char *format, va_list args)
{
    switch(level) {
        case FAT32_LOG_INFO:
            vstm_log(LEVEL_INFO, 0, file, line, format, args);
            break;
        case FAT32_LOG_WARN:
            vstm_log(LEVEL_WARNING, 0, file, line, format, args);
            break;
        case FAT32_LOG_ERROR:
            vstm_log(LEVEL_ERROR, 0, file, line, format, args);
            break;
    }
}

