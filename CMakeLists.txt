cmake_minimum_required(VERSION 3.29)
project(chess_engine C)

set (CMAKE_C_FLAGS "-std=c89")

add_executable(chess_engine main.c
        uint64.c
        chess_engine.h
        board.c
        move_generation.c
        game.c
        test.c
        evaluation.c
        memory_debug.h
        memory_debug.c
        play.c
)
