# Hi 👋, I'm Sergey Makarov

![Profile views](https://komarev.com/ghpvc/?username=sergey030520&label=Profile%20views&color=0e75b6&style=flat)

- 🔭 I’m currently working on [STM32 SDIO + FAT32](https://github.com/Sergey030520/stm32-sdio-fat32.git)
- 👨‍💻 All of my projects are available at [https://github.com/Sergey030520](https://github.com/Sergey030520)

## Languages and Tools

![C](https://github.com/tandpfun/skill-icons/blob/main/icons/C.svg) 
![CMake](https://github.com/tandpfun/skill-icons/blob/main/icons/CMake-Dark.svg) 
![VSCode](https://github.com/tandpfun/skill-icons/blob/main/icons/VSCode-Dark.svg) 
![Git](https://github.com/tandpfun/skill-icons/blob/main/icons/Git.svg) 

# STM32 SDIO + FAT32

## Оглавление

## Оглавление

1. [Описание](#description_project)
2. [Структура проекта и подключение библиотеки FAT32](#manual_connect_library)
3. [Сборка проекта](#build_project)
4. [Тестирование](#testing)
5. [Загрузка на плату](#flash_to_board)
6. [API работы с SD-картой и файлами](#interface_project)
7. [Примеры работы](#example_work_project)
8. [UART Middleware](#uart_middleware)
9. [Фото работы](#project_photos)  
10. [Пример вывода (Log Output)](#log_output)
   
## Описание <a name="description_project"></a>

Проект реализует драйвер SDIO и файловую систему FAT32 для **STM32F407VET6** с нуля, без сторонних библиотек.  
Цель — работа с SD-картами для чтения и записи данных в формате FAT32 в реальном времени.

### Основные возможности

#### Работа с SDIO
- Инициализация SD-карты через SDIO.
- Чтение и запись блоков данных.
- Стирание памяти на SD-карте.
- Поддержка SD, SDHC, SDXC карт.
- Драйверы RTC, Timer, RCC, UART (логирование через DMA), IRQ, Flash, GPIO, LEDC.
- Собственная система логирования.

#### Работа с FAT32
- Создание директорий.
- Удаление файлов и папок (обычное и рекурсивное).
- Форматирование SD-карты.
- Монтирование SD-карты.
- Создание, запись, дозапись, чтение файлов.
- Проверка существования файла или папки.

## Сборка проекта <a name="build_project"></a>

Проект поддерживает сборку с помощью **CMake**.

Пример сборки через CMake (Linux):

```bash
mkdir build
cd build
cmake ..
make
```
Проект был протестирован на STM32F407VET6 с SD-картой 8GB. Все операции чтения/записи корректны.

## Тестирование <a name="testing"></a>

Файл sd_fat32_test.c содержит функции тестирования:
```
int sd_fat32_create_directory_test();
int sd_fat32_write_file_test();
int sd_fat32_write_and_read_test();
int sd_fat32_append_data_test();
```

## Загрузка на плату <a name="flash_to_board"></a>

На Linux перед загрузкой прошивки необходимо установить утилиту `st-flash`:

```
sudo apt install stlink-tools
```

Для загрузки прошивки на STM используется скрипт load_stm_data.sh с утилитой st-flash:
```
./load_stm_data.sh
```

## Работа с SD-картой и файлами
Пример основных функций проекта:
```
int init_sd_fat32();  // инициализация SD-карты и монтирование FAT32
int sd_fat32_open(const char *path, SD_FAT32_File *sf_file, int mode);  // открыть файл
int sd_fat32_exists(const char *path);                                   // проверка существования файла/папки
int sd_fat32_write(SD_FAT32_File *sf_file, const uint8_t *data, uint32_t len); // запись данных
int sd_fat32_flush(SD_FAT32_File *sf_file);                               // сброс буфера на SD-карту
int sd_fat32_close(SD_FAT32_File *sf_file);                               // закрытие файла
```
Примеры работы <a name="example_work_project"></a>
#### Запись данных в файл
```
FAT32_File *file = NULL;
char *path = "/MYDIR/TEST/test.txt";
char *data = "Hello FAT32! This is static test data.\n";

int status = open_file_fat32(path, &file, F_WRITE);
if (status != 0) {
    LOG_INFO("Failed to open file %s (err=%d)", path, status);
    return status;
}

status = write_file_fat32(file, data, strlen(data));
if (status >= 0)
    LOG_INFO("Written to %s: '%s'", path, data);

flush_fat32(file);
close_file_fat32(&file);

```

#### Чтение данных из файла
```
FAT32_File *file = NULL;
char buffer[128];

int status = open_file_fat32(file_path, &file, F_READ);
if (status != 0) {
    LOG_INFO("Failed to open file %s (err=%d)", file_path, status);
    return status;
}

status = read_file_fat32(file, buffer, sizeof(buffer) - 1);
if (status >= 0) {
    buffer[status] = '\0';
    LOG_INFO("Read from %s: '%s'", file_path, buffer);
}

close_file_fat32(&file);
```
#### Создание директории
```
int status = sd_fat32_create_directory("/MYDIR/NEW_FOLDER");
if (status == 0) {
    LOG_INFO("Directory created successfully!");
} else {
    LOG_INFO("Failed to create directory (err=%d)", status);
}
```
#### Проверка существования файла или директории
```
if (sd_fat32_exists("/MYDIR/TEST/test.txt")) {
    LOG_INFO("File exists!");
} else {
    LOG_INFO("File does not exist.");
}
```
## UART Middleware <a name="uart_middleware"></a>

В файлах `middleware/usart_board.c` и `middleware/usart_board.h` реализована функция:

```c
void init_uart();
```
Функция init_uart() конфигурирует USART1: настраивает TX/RX пины, включает DMA для передачи данных, задаёт базовые параметры UART. Если не изменять конфигурацию вручную, используется скорость передачи 115200 бод по умолчанию.

## Фото работы <a name="project_photos"></a>

### 1. Карта вставлена  
При вставленной SD-карте проект успешно инициализируется, создаётся папка и файл на карте.  

<p align="center">
  <img src="docs/images/board_with_sd.jpg" alt="STM32 Board with SD card" width="250"/>
  <img src="docs/images/file_test.png" alt="File created on SD" width="250"/>
  <img src="docs/images/folder_test.png" alt="Folder created on SD" width="250"/>
  <img src="docs/images/root_dir.png" alt="Root directory structure" width="250"/>
</p>  

---

### 2. Карта не вставлена  
При отсутствии SD-карты плата запускается, но не сигнализирует об успешной инициализации, а лог показывает ошибку.  

<p align="center">
  <img src="docs/images/board_without_sd.jpg" alt="STM32 Board without SD card" width="300"/>
</p>

## Пример вывода (Log Output) <a name="log_output"></a>

### 1. Карта вставлена и работает
Пример инициализации драйверов, SD-карты и FAT32, а также создания директорий и записи в файл:  

```
[main.c:31] Драйверы запущены!
[sd_fat32.c:95] RTC initialized and datetime callback set
[SD.c:403] supported VHS (V2.X)![SD.c:459] SD card ready: type=SDSC, OCR=0x000001FE
[SD.c:477] CID: MID 0, OID 289, product , ver 77, SN 1226842223
[SD.c:490] Set new RCA: 22964; state: 2[SD.c:582] CSD version: 2, class command: b5, DSR: 0,[SD.c:600] WRITE_BL_LEN: 9, READ_BL_LEN: 9[SD.c:601] SD capacity: 9
[SD.c:602] C_SIZE: 15359, [SD.c:603] COPY: 0, file format: 0
[SD.c:606] capacity_gb: 3
[SD.c:616] Card version: 4
[SD.c:736] Card [rsa: 22964] select![SD.c:756] Set BUS width 4
[sd_fat32.c:105] SD initialized!
[sd_fat32.c:112] Fat32 initialized!
[/home/sergey-athlete/code/Board/stm/projects/stm32-sdio-fat32/filesystems-ufat32/src/FAT32.c:1051] name new dir: TEST
[sd_fat32_test.c:13] Directory created: /MYDIR/TEST
[/home/sergey-athlete/code/Board/stm/projects/stm32-sdio-fat32/filesystems-ufat32/src/FAT32.c:1051] name new dir: TEST
[sd_fat32_test.c:144] Directory created: /MYDIR/TEST
[sd_fat32_test.c:159] File opened for append: /MYDIR/TEST/test.txt
[/home/sergey-athlete/code/Board/stm/projects/stm32-sdio-fat32/filesystems-ufat32/src/FAT32.c:763] next_cluster: 5, adderess:32824, shift:166
[sd_fat32_test.c:171] Data appended to /MYDIR/TEST/test.txt
[sd_fat32_test.c:185] Read from /MYDIR/TEST/test.txt: 'Hello FAT32! Appending test data.Hello FAT32! Appending test data.Hello FAT32! Appending test data.Hello FAT32! Appending test'
```
### 2. Карта отсутствует
Пример логов при старте без SD-карты:

```
[main.c:31] Драйверы запущены!
[sd_fat32.c:95] RTC initialized and datetime callback set
[SD.c:407] unsupported VHS, fallback to V1.X!
[SD.c:432] ACMD41 send fail, status=-133
[sd_fat32.c:102] Error launch Driver sd: -1
[main.c:34] Error init sd+fat32!(error=-2)
```
