#include "usart_board.h"
#include "usart.h"
#include "dma.h"
#include "board_pins.h"
#include "rcc.h"





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
        .gpiox = USART1_TX_PORT,
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


int usart_adapter(const char *data, int length)
{
    send_data_usart((uint8_t *)data, (uint16_t)length);
    return 0;
}