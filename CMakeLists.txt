cmake_minimum_required(VERSION 2.13)
project(SEfile C)

include_directories(se3)

set(CMAKE_C_STANDARD 99)

file(GLOB SRC "se3/*.c")

add_executable(SEfile-cli SEfile-cli.c wrapper.c SEfile.c ${SRC} wrapper.h SEfile-cli.h)
