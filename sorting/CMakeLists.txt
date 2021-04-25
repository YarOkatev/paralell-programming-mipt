cmake_minimum_required(VERSION 3.17)
project(sorting)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_CXX_COMPILER "mpicxx")
set(CMAKE_CXX_FLAGS "-O3")

add_executable(sorting main.cpp sort.h)
