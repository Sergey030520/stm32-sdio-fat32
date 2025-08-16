#include <stdlib.h>
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "fpu.h"
#include "SD.h"
#include "IRQ.h"
#include "timer.h"
#include "src/FAT.h"
#include "log.h"
#include "memory_map.h"
#include "board_pins.h"

#define DEFAULT_TIMEOUT 1000

int init_rcc();
int init_board();
void init_uart();
int init_fat32(BlockDevice *device);

int clear_sd(uint32_t offset, uint32_t size);
int read_sd(uint8_t *buffer, uint32_t size, uint32_t sector);
int write_sd(const uint8_t *data, uint32_t size, uint32_t sector);
int read_sd_with_dma(uint8_t *buffer, uint32_t size, uint32_t sector);
int write_sd_with_dma(const uint8_t *data, uint32_t size, uint32_t sector);
int usart_adapter(const char *data, int length);
int sd_card_init();

int main()
{
    int status = init_board();
    if (status != 0)
    {
        // ledOn(13, 1);
        goto error;
    }
    LOG_INFO("Драйверы запущены и карта инициализирована!");

    BlockDevice sd_device = {
        .clear = clear_sd,
        .read = read_sd,
        .write = write_sd};
    status = init_fat32(&sd_device);
    if (status != 0)
    {
        goto error;
    }
    LOG_INFO("Fat32 проинициализирована!");

    ledOn(15, 1);
    while (1)
    {
    }

error:
    while (1)
        ;
    return 0;
}

int init_rcc()
{
    RCC_PLLConfig pll_config = {0};
    RCC_BusConfig bus_config = {0};
    RCC_SystemConfig system_config = {0};
    RCC_PeripheralClockConfig peripheral_conf = {0};

    int status = 0;

    pll_config.PLLM = 25;
    pll_config.PLLN = 336;
    pll_config.PLLP = RCC_PLLP_DIV2;
    pll_config.PLLQ = 7;

    pll_config.source = RCC_HSE;

    bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
    bus_config.apb1_prescaler = RCC_APB_DIV4;
    bus_config.apb2_prescaler = RCC_APB_DIV2;
    bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

    system_config.source = RCC_PLL;
    system_config.bus_config = &bus_config;
    system_config.pll_config = &pll_config;

    status = setup_system_config_rcc(&system_config);

    if (status != 0)
    {
        pll_config.PLLM = 16;
        pll_config.PLLN = 336;
        pll_config.PLLP = RCC_PLLP_DIV2;
        pll_config.PLLQ = 7;

        pll_config.source = RCC_HSI;

        bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
        bus_config.apb1_prescaler = RCC_APB_DIV4;
        bus_config.apb2_prescaler = RCC_APB_DIV2;
        bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

        status = setup_system_config_rcc(&system_config);
        if (status != 0)
        {
            bus_config.ahb_prescaler = RCC_NOT_DIV;
            bus_config.apb1_prescaler = RCC_NOT_DIV;
            bus_config.apb2_prescaler = RCC_NOT_DIV;

            system_config.source = RCC_HSI;
            system_config.pll_config = NULL;
            status = setup_system_config_rcc(&system_config);
        }
    }

    peripheral_conf.APB1 = RCC_APB1ENR_TIM2EN;
    peripheral_conf.APB2 = RCC_APB2ENR_USART1EN;
    peripheral_conf.AHB1 = RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_GPIOAEN;
    peripheral_conf.AHB2 = 0;
    peripheral_conf.AHB3 = 0;

    enable_gpio_clock_rcc(&peripheral_conf);
    return status;
}

int init_board()
{
    int status = 0;
    RCC_Frequencies rcc_clocks = {0};

    init_fpu();

    init_irq();

    status = init_rcc();

    init_led();

    if (status != 0)
    {
        ledOn(13, 1);
        return -1;
    }

    init_uart();

    stm_init_log(usart_adapter);

    status = sd_card_init();

    if (status != 0)
    {
        ledOn(13, 1);
        LOG_INFO("Error launch Driver sd: %d", status);
        return -2;
    }
    return 0;
}

int read_sd(uint8_t *buffer, uint32_t size, uint32_t sector)
{
    return read_multi_block_sd(buffer, size / 512, sector, DEFAULT_TIMEOUT);
}
int write_sd(const uint8_t *data, uint32_t size, uint32_t sector)
{
    return write_multi_block_sd((uint8_t *)data, size / 512, sector, DEFAULT_TIMEOUT);
}

int read_sd_with_dma(uint8_t *buffer, uint32_t size, uint32_t sector)
{
    return read_blocks_dma(buffer, size / 512, sector, DEFAULT_TIMEOUT);
}

int write_sd_with_dma(const uint8_t *data, uint32_t size, uint32_t sector)
{
    return write_blocks_dma((uint8_t *)data, size / 512, sector, DEFAULT_TIMEOUT);
}

int usart_adapter(const char *data, int length)
{
    send_data_usart((uint8_t *)data, (uint16_t)length);
    return 0;
}

void init_uart()
{
    DMA_Config dma_tx_config = {
        .dma = DMA2,          // DMA2 для USART1_TX
        .stream = STREAM_7,   // Stream 7
        .channel = CHANNEL_4, // канал для USART1_TX
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_MSIZE_8BITS,
        .periph_size = DMA_PSIZE_8BITS,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0};

    GPIO_PinConfig_t tx_pin_config = {
        .gpiox = GPIOA_REG,
        .pin = USART1_TX_PIN,
        .mode = GPIO_MODER_ALTERNATE,
        .speed = GPIO_SPEED_100MHz,
        .pull = GPIO_PULL_NONE,
        .output = GPIO_OUTPUT_PUSHPULL,
        .af = GPIO_AF7,
    };
    GPIO_PinConfig_t rx_pin_config = {
        .gpiox = GPIOA_REG,
        .pin = USART1_RX_PIN,
        .mode = GPIO_MODER_ALTERNATE,
        .speed = GPIO_SPEED_100MHz,
        .pull = GPIO_PULL_NONE,
        .output = GPIO_OUTPUT_PUSHPULL,
        .af = GPIO_AF7,
    };

    // Основная конфигурация UART
    UART_Config_t uart1_config = {
        .usart = USART1_REG,
        .baud_rate = UART_BAUDRATE_115200,
        .tx_mode = UART_MODE_POLLING,
        .rx_mode = UART_MODE_POLLING,
        .dma_tx = dma_tx_config,   // DMA конфигурация для TX
        .dma_rx = {0},             // RX DMA не используется
        .tx_port = &tx_pin_config, // Пин TX
        .rx_port = &rx_pin_config  // Пин RX (можно NULL, если RX отключен)
    };

    RCC_Frequencies rcc_clocks = {0};

    get_clock_frequencies(&rcc_clocks);

    setup_uart(&uart1_config, rcc_clocks.APB2_Freq);
}

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

    int status = mount_fat32(device);
    if (status != 0)
    {
        // Попытка форматирования устройства
        status = formatted_fat32(device, SIZE_8GB);
        if (status != 0)
        {
            return -2;
        }

        // Повторная попытка монтирования после форматирования
        status = mount_fat32(device);
        if (status != 0)
        {
            return -3;
        }
    }

    return 0;
}

int clear_sd(uint32_t offset, uint32_t size)
{
    uint32_t address_start = offset;
    uint32_t address_end = offset + size - 1; 
   return erase_sd(address_start, address_end);
}