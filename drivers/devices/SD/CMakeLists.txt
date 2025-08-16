
SET(LIBRARY_SDIO_NAME driver_sd)

add_library(${LIBRARY_SDIO_NAME} STATIC
        SD.h SD.c
)

target_include_directories(${LIBRARY_SDIO_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${LIBRARY_SDIO_NAME} PUBLIC drivers_core logger)