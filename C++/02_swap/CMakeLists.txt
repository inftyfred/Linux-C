cmake_minimum_required(VERSION 3.22.0)

project(SWAP)

include_directories(./include)

add_executable(swap_cmake ./main.cpp ./src/swap.cpp)