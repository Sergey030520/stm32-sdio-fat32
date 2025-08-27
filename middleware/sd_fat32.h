#pragma once


#include <stdint.h>



int clear_sd(uint32_t sector_num, uint32_t sector_count, uint32_t sector_size);
int read_sd(uint8_t *buffer, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size);
int write_sd(const uint8_t *data, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size);
int init_sd_fat32();
int sd_card_init();