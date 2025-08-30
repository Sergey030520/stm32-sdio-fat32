
#pragma once 

#include "fat32/log_fat32.h"
#include <stdarg.h>
#include <stdio.h>



void fat32_stm_log(Fat32LogLevel level, const char *file, int line, const char *format, va_list args);