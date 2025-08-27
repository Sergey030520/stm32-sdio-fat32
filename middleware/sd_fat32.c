#include "sd_fat32.h"
#include "src/FAT.h"
#include "SD.h"
#include <stdio.h>
#include "log.h"
#include "led.h"
#include "timer.h"
#include "src/pool_memory.h"


#define DEFAULT_TIMEOUT 1000000
#define BLOCK_SIZE 512

BlockDevice sd_device = {
    .clear = clear_sd,
    .read = read_sd,
    .write = write_sd,
    .block_size = 512,
};

int init_fat32(BlockDevice *device);
int sd_card_init();

int write_safe_sd(const uint8_t *data, uint32_t count_blocks, uint32_t block_start, uint32_t timeout);
int read_safe_sd(uint8_t *buffer, uint32_t count_blocks, uint32_t block_start, uint32_t timeout);


int sd_card_init()
{
    int status = init_sd();
    if (status != 0)
    {
        return -1;
    }

    status = init_sd_card();
    if (status != 0)
    {
        return -2;
    }
    return 0;
}

int init_fat32(BlockDevice *device)
{
    if (device == NULL)
    {
        return -1;
    }

    int status = -1; 
    // mount_fat32(device);
    LOG_INFO("SD card is not formatted");
    if (status != 0)
    {
        LOG_INFO("Formatting SD card");
        status = formatted_fat32(device, SIZE_8GB);
        if (status != 0)
        {
            return -2;
        }
        LOG_INFO("Retrying SD card mount");

        status = mount_fat32(device);
        if (status != 0)
        {
            return -3;
        }
    }

    return 0;
}

int init_sd_fat32()
{
    int status = sd_card_init();
    if (status != 0)
    {
        LOG_INFO("Error launch Driver sd: %d", status);
        return -1;
    }
    LOG_INFO("SD проинициализирована!");

    status = init_fat32(&sd_device);
    if (status != 0)
    {
        return -2;
    }
    LOG_INFO("Fat32 проинициализирована!");
    return 0;
}


int read_safe_sd(uint8_t *buffer, uint32_t count_blocks, uint32_t block_start, uint32_t timeout)
{
    int attempts = 5;
    int status;
    while (attempts--)
    {
        status = read_multi_block_sd(buffer, count_blocks, block_start, timeout);
        if (status == 0)
            return 0;
        delay_timer(20); 
    }
    return status;
}

int read_sd(uint8_t *buffer, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size)
{
    uint32_t block_size = sd_device.block_size;
    uint32_t block_start = (start_sector * sector_size) / block_size;
    uint32_t total_bytes = sector_count * sector_size;

    uint32_t full_blocks = total_bytes / block_size;
    uint32_t remaining_bytes = total_bytes % block_size;

    LOG_INFO("READ_SD: sector_count: %d, start_sector: %d, sector_size: %d\r\n",
             sector_count, start_sector, sector_size);
    LOG_INFO("READ_SD: block_size: %d, block_start: %d, total_bytes: %d, full_blocks: %d, remaining_bytes: %d\r\n",
             block_size, block_start, total_bytes, full_blocks, remaining_bytes);

    if (full_blocks > 0)
    {
        int status = read_safe_sd(buffer, full_blocks, block_start, DEFAULT_TIMEOUT);
        if (status != 0) return status;
    }

    if (remaining_bytes > 0)
    {
        uint8_t *tmp_block = pool_alloc(block_size);
        if (tmp_block == NULL) return POOL_ERR_ALLOCATION_FAILED;

        int status = read_safe_sd(tmp_block, 1, block_start + full_blocks, DEFAULT_TIMEOUT);
        if (status != 0) 
        {
            pool_free_region(tmp_block, block_size);
            return status;
        }

        memcpy(buffer + full_blocks * block_size, tmp_block, remaining_bytes);
        pool_free_region(tmp_block, block_size);
    }

    return 0;
}

int write_safe_sd(const uint8_t *data, uint32_t count_blocks, uint32_t block_start, uint32_t timeout)
{
    int attempts = 3;
    int status;
    while (attempts--)
    {
        status = write_multi_block_sd(data, count_blocks, block_start, timeout);
        if (status == 0)
            return 0;
        
        else{ LOG_INFO("err_status:%d\r\n", status);}
        delay_timer(100);
    }
    return status;
}

int write_sd(const uint8_t *data, uint32_t sector_count, uint32_t start_sector, uint32_t sector_size)
{
    uint32_t block_size = sd_device.block_size;
    uint32_t block_start = (start_sector * sector_size) / block_size;
    uint32_t total_bytes = sector_count * sector_size;

    uint32_t full_blocks = total_bytes / block_size;
    uint32_t remaining_bytes = total_bytes % block_size;

    LOG_INFO("WRITE_SD: sector_count: %d,start_sector:%d,sector_size:%d\r\n",
             sector_count, start_sector, sector_size);
    LOG_INFO("WRITE_SD: block_size: %d,block_start:%d,total_bytes:%d, full_blocks:%d, remaining_bytes:%d\r\n",
             block_size, block_start, total_bytes, full_blocks, remaining_bytes);

    int status = 0;

    if (full_blocks > 0)
    {
        status = write_safe_sd(data, full_blocks, block_start, DEFAULT_TIMEOUT);
        if (status != 0) return status;
    }

    if (remaining_bytes > 0)
    {
        uint8_t *tmp_block = pool_alloc(block_size);
        if (!tmp_block) return -1;

        status = read_multi_block_sd(tmp_block, 1, block_start + full_blocks, DEFAULT_TIMEOUT);
        if (status != 0)
        {
            pool_free_region(tmp_block, block_size);
            return status;
        }

        memcpy(tmp_block, data + full_blocks * block_size, remaining_bytes);

        status = write_safe_sd(tmp_block, 1, block_start + full_blocks, DEFAULT_TIMEOUT);
        pool_free_region(tmp_block, block_size);

        if (status != 0) return status;
    }

    return 0;
}


int clear_sd(uint32_t sector_num, uint32_t sector_count, uint32_t sector_size)
{
    uint32_t block_start = (sector_num * sector_size) / sd_device.block_size;
    uint32_t offset = (sector_count * sector_size + sd_device.block_size - 1) / sd_device.block_size;
    return erase_sd(block_start, offset);
}
/*
status = sd_card_init();
    if(status != 0){
        LOG_INFO("SD драйвер не запущен! (error=%d)\r\n",status);
        goto error;
    }

    // status = erase_sd(0, 65568);
    status = clear_sd(0, 65568);
    if (status != 0)
    {
        LOG_INFO("Erase error: %d\r\n", status);
    }
    else
    {
        LOG_INFO("Data erase!\r\n");
    }
    uint8_t buff[512] = {0};
    for (int i = 0; i < 512; i += 2)
    {
        *((uint16_t *)&buff[i]) = 0x55AA;
    }
    // status = write_multi_block_sd(buff, 1, 0,1000000);

    status = write_sd(buff, 1, 0);

    if (status != 0)
    {
        LOG_INFO("Write error: %d\r\n", status);
    }
    else
    {
        LOG_INFO("Data write!\r\n");
    }

    uint8_t buff_rx[512] = {0};
    // status = read_multi_block_sd(buff_rx, 1, 0, 1000000);
    status = read_sd(buff_rx, 1, 0);
    if (status != 0)
    {
        LOG_INFO("Read error: %d\r\n", status);
    }
    else
    {
        int count = 0;
        for (int i = 0; i < 512; i += 2)
        {
            if (buff_rx[i] != buff[i])
            {
                count += 1;
            }
        }
        LOG_INFO("Compare with: %d\r\n", count);
    }
*/