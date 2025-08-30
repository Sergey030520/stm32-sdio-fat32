SET(LIBRARY_ALLOCATOR allocator)

add_library(${LIBRARY_ALLOCATOR} STATIC
    pool_memory.h pool_memory.c
)

target_include_directories(${LIBRARY_ALLOCATOR} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})