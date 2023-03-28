cmake_minimum_required(VERSION 3.24)
project(SO_CPU_Planner C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(SO_CPU_Planner
        client.c
        jobstruct.h
        server.c cpuScheduler.c)
