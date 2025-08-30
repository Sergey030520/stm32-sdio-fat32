#include "sd_fat32_test.h"
#include "sd_fat32.h"
#include "fat32/FAT32.h"
#include "log.h"
#include <string.h>


int sd_fat32_create_directory_test()
{
    char *path = "/MYDIR/TEST";
    int status = mkdir_fat32(path);
    if (status == 0)
    {
        LOG_INFO("Directory created: %s\r\n", path);
    }
    else
    {
        LOG_INFO("Failed to create directory %s (err=%d)\r\n", path, status);
    }
    return status;
}

int sd_fat32_create_file_test()
{
    char *path = "/MYDIR/TEST/test.txt";
    FAT32_File *file = NULL;
    int status = open_file_fat32(path, &file, F_WRITE);
    if (status == 0)
    {
        LOG_INFO("File created: %s", path);
        flush_fat32(file);
        close_file_fat32(file);
    }
    else
    {
        LOG_INFO("Failed to create file %s (err=%d)\r\n", path, status);
    }
    return status;
}

int sd_fat32_write_file_test()
{
    char *path = "/MYDIR/TEST/test.txt";
    char *data = "Hello FAT32! This is static test data.\n";
    FAT32_File *file = NULL;

    int status = open_file_fat32(path, &file, F_WRITE);
    if (status != 0)
    {
        LOG_INFO("Failed to open file %s for write (err=%d)\r\n", path, status);
        return status;
    }

    status = write_file_fat32(file, data, strlen(data));
    if (status == 0)
    {
        LOG_INFO("Written to %s: '%s'", path, data);
    }
    else
    {
        LOG_INFO("Failed to write to %s (err=%d)\r\n", path, status);
    }

    flush_fat32(file);
    close_file_fat32(file);
    return status;
}

int sd_fat32_write_and_read_test()
{
    char *dir_path = "/MYDIR/TEST";
    char *file_path = "/MYDIR/TEST/test.txt";
    char *data = "Hello FAT32! This is static test data.\n";
    char buffer[128] = {0};
    int status;

    // 1. Создание директории
    status = mkdir_fat32(dir_path);
    if (status == 0)
    {
        LOG_INFO("Directory created: %s\r\n", dir_path);
    }
    else
    {
        LOG_INFO("Directory %s may already exist or failed (err=%d)\r\n", dir_path, status);
    }

    // 2. Создание файла
    FAT32_File *file = NULL;
    status = open_file_fat32(file_path, &file, F_WRITE);
    if (status != 0)
    {
        LOG_INFO("Failed to create file %s (err=%d)\r\n", file_path, status);
        return status;
    }
    LOG_INFO("File created: %s\r\n", file_path);

    // 3. Запись данных
    status = write_file_fat32(file, data, strlen(data));
    if (status != 0)
    {
        LOG_INFO("Failed to write to %s (err=%d)\r\n", file_path, status);
        close_file_fat32(file);
        return status;
    }
    flush_fat32(file);
    close_file_fat32(file);
    LOG_INFO("Data written to %s\r\n", file_path);

    // 4. Чтение данных обратно
    status = open_file_fat32(file_path, &file, F_READ);
    if (status != 0)
    {
        LOG_INFO("Failed to open file %s for read (err=%d)\r\n", file_path, status);
        return status;
    }

    status = read_file_fat32(file, buffer, sizeof(buffer) - 1);
    if (status >= 0)
    {
        buffer[status] = '\0';
        LOG_INFO("Read from %s: '%s'\r\n", file_path, buffer);
    }
    else
    {
        LOG_INFO("Failed to read from %s (err=%d)\r\n", file_path, status);
    }

    close_file_fat32(file);
    return status;
}
