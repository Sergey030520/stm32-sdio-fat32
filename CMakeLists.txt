cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

project(SDIO_FAT32 C ASM)

set(EXECUTABLE ${PROJECT_NAME}.elf)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/conf_mem_stm32f4/STM32F407VETX_FLASH.ld)


add_subdirectory(drivers)
add_subdirectory(logger)
add_subdirectory(filesystem)

set(ARM_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fdata-sections
    -ffunction-sections
    -fsingle-precision-constant
    -Wall
    -O0
    -g3
)


add_executable(${EXECUTABLE} 
    startup/startup_stm32f407vetx.s
    main.c
)



target_compile_options(drivers_core PRIVATE ${ARM_FLAGS})
target_compile_options(logger PRIVATE ${ARM_FLAGS})
target_compile_options(driver_sd PRIVATE ${ARM_FLAGS})
target_compile_options(fat32 PRIVATE ${ARM_FLAGS})
target_compile_options(${EXECUTABLE} PRIVATE ${ARM_FLAGS})


target_link_libraries(${EXECUTABLE} PRIVATE
        drivers_core
        logger
        driver_sd
        fat32
        m
)


target_link_options(${EXECUTABLE} PRIVATE
        -T${LINKER_FILE}
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -lc
        -lm
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )



add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
        )

add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        )
