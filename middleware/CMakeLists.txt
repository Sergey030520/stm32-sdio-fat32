add_library(middleware
    usart_board.h usart_board.c    
    sd_fat32.h sd_fat32.c
    sd_fat32_test.h sd_fat32_test.c
    log_stm.h log_stm.c
)

target_include_directories(middleware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware
    ${CMAKE_SOURCE_DIR}/drivers_core
    ${CMAKE_SOURCE_DIR}/allocator
    ${CMAKE_SOURCE_DIR}/logger
    ${CMAKE_SOURCE_DIR}/driver_sd
)

target_link_libraries(middleware PRIVATE
    allocator
    drivers_core
    logger
    driver_sd
    fat32_lib
)