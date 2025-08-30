#include "sd_fat32.h"
#include "SD.h"
#include <stdio.h>
#include "log.h"
#include "led.h"
#include "timer.h"
#include "pool_memory.h"
#include "fat32/fat32_alloc.h"
#include "log_stm.h"

#define DEFAULT_TIMEOUT 1000000
#define BLOCK_SIZE 512

Fat32Allocator fat32_allocator = {0};

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

    fat32_set_logger(stm_log);

    fat32_allocator.alloc = pool_alloc;
    fat32_allocator.free = pool_free_region;
    fat32_allocator.allocator_init = pool_init;
    fat32_allocator_init(&fat32_allocator);

    int status = mount_fat32(device);
    if (status != 0)
    {

        LOG_INFO("SD card is not formatted");
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

    // LOG_INFO("READ_SD: sector_count: %d, start_sector: %d, sector_size: %d\r\n",
    //  sector_count, start_sector, sector_size);
    // LOG_INFO("READ_SD: block_size: %d, block_start: %d, total_bytes: %d, full_blocks: %d, remaining_bytes: %d\r\n",
    //  block_size, block_start, total_bytes, full_blocks, remaining_bytes);

    if (full_blocks > 0)
    {
        int status = read_safe_sd(buffer, full_blocks, block_start, DEFAULT_TIMEOUT);
        if (status != 0)
            return status;
    }

    if (remaining_bytes > 0)
    {
        uint8_t *tmp_block = pool_alloc(block_size);
        if (tmp_block == NULL)
            return POOL_ERR_ALLOCATION_FAILED;

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

        else
        {
            LOG_INFO("err_status:%d\r\n", status);
        }
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

    // LOG_INFO("WRITE_SD: sector_count: %d,start_sector:%d,sector_size:%d\r\n",
    //          sector_count, start_sector, sector_size);
    // LOG_INFO("WRITE_SD: block_size: %d,block_start:%d,total_bytes:%d, full_blocks:%d, remaining_bytes:%d\r\n",
    //          block_size, block_start, total_bytes, full_blocks, remaining_bytes);

    int status = 0;

    if (full_blocks > 0)
    {
        status = write_safe_sd(data, full_blocks, block_start, DEFAULT_TIMEOUT);
        if (status != 0)
            return status;
    }

    if (remaining_bytes > 0)
    {
        uint8_t *tmp_block = pool_alloc(block_size);
        if (!tmp_block)
            return -1;

        status = read_multi_block_sd(tmp_block, 1, block_start + full_blocks, DEFAULT_TIMEOUT);
        if (status != 0)
        {
            pool_free_region(tmp_block, block_size);
            return status;
        }

        memcpy(tmp_block, data + full_blocks * block_size, remaining_bytes);

        status = write_safe_sd(tmp_block, 1, block_start + full_blocks, DEFAULT_TIMEOUT);
        pool_free_region(tmp_block, block_size);

        if (status != 0)
            return status;
    }

    return 0;
}

int clear_sd(uint32_t sector_num, uint32_t sector_count, uint32_t sector_size)
{
    uint32_t block_start = (sector_num * sector_size) / sd_device.block_size;
    uint32_t offset = (sector_count * sector_size + sd_device.block_size - 1) / sd_device.block_size;
    return erase_sd(block_start, offset);
}

int sd_fat32_open(const char *path, SD_FAT32_File *sf_file, int mode)
{
    sf_file->file = NULL;
    sf_file->index = 0;
    int status = open_file_fat32(path, &sf_file->file, mode);
    if (status == 0)
    {
        LOG_INFO("File opened: %s", path);
    }
    else
    {
        LOG_INFO("Failed to open file %s (err=%d)", path, status);
    }
    return status;
}

int sd_fat32_exists(const char *path)
{
    int status = path_exists_fat32(path);
    LOG_INFO("%s %s", path, status == 0 ? "exists" : "does not exist");
    return status;
}

int sd_fat32_write(SD_FAT32_File *sf_file, const uint8_t *data, uint32_t len)
{
    uint32_t remaining = len;
    const uint8_t *ptr = data;

    while (remaining > 0)
    {
        uint32_t space = SD_FAT32_BUFFER_SIZE - sf_file->index;
        uint32_t to_copy = remaining < space ? remaining : space;
        memcpy(&sf_file->buffer[sf_file->index], ptr, to_copy);
        sf_file->index += to_copy;
        ptr += to_copy;
        remaining -= to_copy;

        if (sf_file->index == SD_FAT32_BUFFER_SIZE)
        {
            int status = write_file_fat32(sf_file->file, sf_file->buffer, SD_FAT32_BUFFER_SIZE);
            if (status != 0)
            {
                LOG_INFO("Write to SD failed (err=%d)", status);
                return status;
            }
            sf_file->index = 0;
        }
    }
    return 0;
}

int sd_fat32_flush(SD_FAT32_File *sf_file)
{
    if (sf_file->index > 0)
    {
        int status = write_file_fat32(sf_file->file, sf_file->buffer, sf_file->index);
        if (status != 0)
        {
            LOG_INFO("Flush failed (err=%d)", status);
            return status;
        }
        sf_file->index = 0;
        LOG_INFO("Buffer flushed to SD");
    }
    return 0;
}

int sd_fat32_close(SD_FAT32_File *sf_file)
{
    sd_fat32_flush(sf_file);
    int status = close_file_fat32(sf_file->file);
    if (status != 0)
    {
        LOG_INFO("Close failed (err=%d)", status);
    }
    else
    {
        LOG_INFO("File closed");
    }
    return status;
}