#include <stdint.h>

#pragma once

// CCR
#define DMA_CHSEL(channel) (channel << 25)
#define DMA_MBURST(value) (value << 23)
#define DMA_PBURST(value) (value << 21)
#define DMA_CT (0b1 << 19)  // Current target (only in double buffer mode)
#define DMA_DBM (0b1 << 18) // Double buffer mode
#define DMA_PL_LOW (0b00 << 16)
#define DMA_PL_MEDIUM (0b01 << 16)
#define DMA_PL_HIGH (0b10 << 16)
#define DMA_PL_VERY_HIGH (0b11 << 16)
#define DMA_PINCOS (0b1 << 15)
#define DMA_MSIZE_8BITS (0x00 << 13)
#define DMA_MSIZE_16BITS (0x01 << 13)
#define DMA_MSIZE_32BITS (0x10 << 13)
#define DMA_PSIZE_8BITS (0x00 << 11)
#define DMA_PSIZE_16BITS (0x01 << 11)
#define DMA_PSIZE_32BITS (0x10 << 11)
#define DMA_MINC (0x1 << 10)
#define DMA_PINC (0x1 << 9)
#define DMA_CIRC (0x1 << 8)
#define DMA_PERIPH_TO_MEM (0b00 << 6)
#define DMA_MEM_TO_PERIPH (0b01 << 6)
#define DMA_MEM_TO_MEM (0b10 << 6)
#define DMA_PFCTR (0x1 << 5)//Переферия указывает количество передаваемых данных
#define DMA_TCIE (0x1 << 4)
#define DMA_HTIE (0x1 << 3)
#define DMA_TEIE (0x1 << 2)
#define DMA_DMEIE (0x1 << 1)
#define DMA_EN (0x1 << 0)


// FCR

#define DMA_FCR_FEIE (0x1 << 7)
#define DMA_FCR_DMDIS (0x1 << 2)
#define DMA_FCR_FTH(threshold) ((uint32_t)(threshold << 0))

typedef enum{
    FTH_1_4_FIFO = 0x0,
    FTH_1_2_FIFO = 0x1,
    FTH_3_4_FIFO = 0x2,
    FTH_FULL_FIFO = 0x3,
} DMA_FTH;

// SR
#define DMA_ISR_FEIFx(stream)  (0x1 << (stream * 6 + ((uint16_t)(stream / 2) * 4)))      // FIFO error interrupt
#define DMA_ISR_DMEIFx(stream) (0x1 << (stream * 6 + ((uint16_t)(stream / 2) * 4) + 2)) // Direct mode error interrupt
#define DMA_ISR_TEIFx(stream)  (0x1 << (stream * 6 + ((uint16_t)(stream / 2) * 4) + 3)) // Transfer error interrupt
#define DMA_ISR_HTIFx(stream)  (0x1 << (stream * 6 + ((uint16_t)(stream / 2) * 4) + 4)) // Half transfer interrupt
#define DMA_ISR_TCIFx(stream)  (0x1 << (stream * 6 + ((uint16_t)(stream / 2) * 4) + 5)) // Transfer complete interrupt

// IFSR - clear register
#define DMA_IFSR_FEIFx(stream)  DMA_ISR_FEIFx(stream)
#define DMA_IFSR_DMEIFx(stream) DMA_ISR_DMEIFx(stream)
#define DMA_IFSR_TEIFx(stream)  DMA_ISR_TEIFx(stream)
#define DMA_IFSR_HTIFx(stream)  DMA_ISR_HTIFx(stream)
#define DMA_IFSR_TCIFx(stream)  DMA_ISR_TCIFx(stream)

typedef enum
{
    CHANNEL_0 = 0,
    CHANNEL_1 = 1,
    CHANNEL_2 = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
} DMA_Channel;

typedef enum
{
    STREAM_0 = 0,
    STREAM_1 = 1,
    STREAM_2 = 2,
    STREAM_3 = 3,
    STREAM_4 = 4,
    STREAM_5 = 5,
    STREAM_6 = 6,
    STREAM_7 = 7,
} DMA_Stream;

typedef struct
{
    uint32_t CCR;   // конфигурация канала
    uint32_t CNDTR; // кол-во передаваемых данных
    uint32_t CPAR;  // адрес периферии канала
    uint32_t CMAR0; // адрес памяти канала
    uint32_t CMAR1; // used in case of Double buffer mode
    uint32_t FCR;   // FIFO control
} DMA_Stream_Settings;

typedef struct
{
    uint32_t LISR;
    uint32_t HISR;
    uint32_t LIFCR;
    uint32_t HIFCR;
    DMA_Stream_Settings streams[8];
} DMA_Type;

typedef enum {
    DMA_DIR_PERIPH_TO_MEM = 0,
    DMA_DIR_MEM_TO_PERIPH = 1,
} DMA_Direction;

typedef struct {
    DMA_Type *dma;
    DMA_Stream stream;
    DMA_Channel channel;
    DMA_Direction direction;
    uint32_t peripheral_addr;
    uint32_t mem_size; 
    uint32_t periph_size;
    uint8_t inc_mem;
    uint8_t inc_periph;
    uint8_t circular;
} DMA_Config;

extern DMA_Type *DMA2;
extern DMA_Type *DMA1;

typedef void (* dma_callback_t) (void);


void dma_init(DMA_Config *cfg);
void dma_send(DMA_Config *cfg, void *buffer, uint32_t length);

void set_enabled_dma(DMA_Type *dma, DMA_Stream stream, uint8_t enable);
void reset_flags_dma(DMA_Type *dma, DMA_Stream stream);


void DMA2_Stream3_IRQHandler();
void DMA2_Stream4_IRQHandler();
void DMA2_Stream7_IRQHandler();

