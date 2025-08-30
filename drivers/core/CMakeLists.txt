SET(LIBRARY_CORE_NAME drivers_core)


add_library(${LIBRARY_CORE_NAME} STATIC
    memory_map.h
    utils.h utils.c
    fpu.h fpu.c
    flash.h
    gpio.h gpio.c
    rcc.h rcc.c
    led.h led.c
    pwr.h pwr.c
    timer.h timer.c
    usart.h usart.c
    RTC.h RTC.c
    dma.h dma.c
    IRQ.h IRQ.c
    sysmem.c
    syscalls.c
    sdio.h sdio.c
    board_pins.h
)

target_include_directories(${LIBRARY_CORE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})