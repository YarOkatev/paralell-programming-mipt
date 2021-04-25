cmake_minimum_required(VERSION 3.17)
project(ParProg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_CXX_COMPILER "mpicxx")

add_executable(ParProg main.cpp)