cmake_minimum_required(VERSION 3.22.0)

project(SOLIDER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall")

include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(./src SRC)

add_executable(solider main.cpp ${SRC})
