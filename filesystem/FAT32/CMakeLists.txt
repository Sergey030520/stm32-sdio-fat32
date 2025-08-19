SET(LIBRARY_FAT32_NAME fat32)

add_library(${LIBRARY_FAT32_NAME} STATIC
    src/FAT.h src/FAT.c
    src/file_utils.h src/file_utils.c
    src/DateTime.h src/DateTime.c
    src/pool_memory.h src/pool_memory.c
    src/log_fat32.h src/log_stm.c
)

target_include_directories(${LIBRARY_FAT32_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${LIBRARY_FAT32_NAME} PUBLIC logger)



