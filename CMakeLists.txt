cmake_minimum_required(VERSION 4.2)
project(Programacao_Multithreads C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Programacao_Multithreads main.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

target_link_libraries(Programacao_Multithreads PRIVATE Threads::Threads)