#pragma once


#include <stdint.h>
#include "src/FAT.h"


#define SD_FAT32_BUFFER_SIZE 2560  


typedef struct {
    FAT32_File *file;
    uint8_t buffer[SD_FAT32_BUFFER_SIZE];
    uint32_t index;  
} SD_FAT32_File;

int clear_sd(uint32_t sector_num, uint32_t sector_count, uint32_t sector_size);
int read_sd(uint8_t *buffer, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size);
int write_sd(const uint8_t *data, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size);
int init_sd_fat32();
int sd_card_init();


int sd_fat32_open(const char *path, SD_FAT32_File *sf_file, int mode);


int sd_fat32_exists(const char *path);


int sd_fat32_write(SD_FAT32_File *sf_file, const uint8_t *data, uint32_t len);


int sd_fat32_flush(SD_FAT32_File *sf_file);


int sd_fat32_close(SD_FAT32_File *sf_file);