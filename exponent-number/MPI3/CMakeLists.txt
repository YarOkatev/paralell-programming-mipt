cmake_minimum_required(VERSION 3.17)
project(exp_long_mpi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_CXX_COMPILER "mpicxx")

add_executable(exp_long_mpi main.cpp LongDouble.cpp LongDouble.h)