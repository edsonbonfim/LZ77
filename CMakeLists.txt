cmake_minimum_required(VERSION 3.10)
project(LZ77 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(comprimir
        lz77.c
        lz77.h
        comprimir.c)

add_executable(descomprimir
        lz77.c
        lz77.h
        descomprimir.c)

target_link_libraries(comprimir m)
target_link_libraries(descomprimir m)