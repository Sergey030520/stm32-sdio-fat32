#include "dma.h"
#include "memory_map.h"
#include <stdio.h>

DMA_Type *DMA1 = DMA1_REG;
DMA_Type *DMA2 = DMA2_REG;

// dma_callback_t

#define DMA_STREAM_COUNT 8

uint8_t dma_channels_busy[2][DMA_STREAM_COUNT];

void reset_flags_dma(DMA_Type *dma, DMA_Stream stream)
{
    if (dma == NULL)
        return;

    uint32_t *ifcr = (stream >= 4) ? &dma->HIFCR : &dma->LIFCR;

    *ifcr = DMA_IFSR_FEIFx(stream)  | 
            DMA_IFSR_DMEIFx(stream) | 
            DMA_IFSR_TEIFx(stream)  | 
            DMA_IFSR_HTIFx(stream)  | 
            DMA_IFSR_TCIFx(stream);
}


void set_enabled_dma(DMA_Type *dma, DMA_Stream stream, uint8_t enable)
{
    if (dma == NULL)
        return;

    if (enable)
    {
        dma->streams[stream].CCR |= DMA_EN;
        while (!(dma->streams[stream].CCR & DMA_EN))
            ;
    }
    else
    {
        dma->streams[stream].CCR &= ~DMA_EN;
        while (dma->streams[stream].CCR & DMA_EN)
            ;
    }
}

void dma_init(DMA_Config *cfg)
{
    DMA_Stream_Settings *stream_cfg = &cfg->dma->streams[cfg->stream];

    // Отключаем поток перед настройкой
    stream_cfg->CCR &= ~DMA_EN;
    while (stream_cfg->CCR & DMA_EN)
        ;

    stream_cfg->CNDTR = 0;
    stream_cfg->CMAR0 = 0;
    stream_cfg->CPAR = cfg->peripheral_addr;

    stream_cfg->CCR = DMA_CHSEL(cfg->channel) |
                      cfg->mem_size |
                      cfg->periph_size |
                      (cfg->inc_mem ? DMA_MINC : 0) |
                      (cfg->inc_periph ? DMA_PINC : 0) |
                      (cfg->circular ? DMA_CIRC : 0) |
                      (cfg->direction == DMA_DIR_MEM_TO_PERIPH ? DMA_MEM_TO_PERIPH : DMA_PERIPH_TO_MEM);
}

uint8_t is_dma_channel_busy(uint8_t dma_idx, DMA_Channel channel)
{
    return dma_channels_busy[dma_idx][channel];
}

void dma_send(DMA_Config *cfg, void *buffer, uint32_t length)
{
    if (cfg == NULL)
        return;

    DMA_Stream_Settings *stream_cfg = &cfg->dma->streams[cfg->stream];

    uint8_t dma_idx = (cfg->dma == DMA2_REG) ? 1 : 0;
    while (is_dma_channel_busy(dma_idx, cfg->channel))
        ;

    stream_cfg->CCR &= ~DMA_EN;
    while (stream_cfg->CCR & DMA_EN)
        ;

    reset_flags_dma(cfg->dma, cfg->stream);

    stream_cfg->CMAR0 = (uint32_t)buffer;
    stream_cfg->CNDTR = length;

    stream_cfg->CCR |= DMA_EN;
}

void DMA2_Stream7_IRQHandler()
{
    DMA_Stream_Settings *stream = &DMA2->streams[7];
    DMA_Type *dma = DMA2_REG;

    // Проверяем флаг Transfer Complete для Stream7
    if (dma->HISR & DMA_ISR_TCIFx(7))
    {
        // Сбрасываем флаг TC
        dma->HIFCR |= DMA_IFSR_TCIFx(7);

        // Отключаем поток DMA
        stream->CCR &= ~DMA_EN;

        // Сигнализируем, что DMA освободился
        dma_channels_busy[2][CHANNEL_7] = 0;
    }

    // Проверяем возможные ошибки DMA (опционально)
    if (dma->HISR & DMA_ISR_TEIFx(7)) // Transfer Error
    {
        dma->HIFCR |= DMA_IFSR_TEIFx(7);
        dma_channels_busy[2][CHANNEL_7] = 0;
    }

    if (dma->HISR & DMA_ISR_DMEIFx(7)) // Direct Mode Error
    {
        dma->HIFCR |= DMA_IFSR_DMEIFx(7);
        dma_channels_busy[2][CHANNEL_7] = 0;
    }

    if (dma->HISR & DMA_ISR_FEIFx(7)) // FIFO Error
    {
        dma->HIFCR |= DMA_IFSR_FEIFx(7);
        dma_channels_busy[2][CHANNEL_7] = 0;
    }

    if (dma->HISR & DMA_ISR_HTIFx(7)) // Half Transfer (опционально)
    {
        dma->HIFCR |= DMA_IFSR_HTIFx(7);
    }
}
