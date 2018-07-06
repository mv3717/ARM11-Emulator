cmake_minimum_required(VERSION 3.10)
project(HaikuBerry C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(HaikuBerry
        grammar.c
        grammar.h
        haiku.c
        haiku.h
        main.c
        parser.c
        parser.h)
