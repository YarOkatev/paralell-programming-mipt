cmake_minimum_required(VERSION 3.17)
project(diffusion_equation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "mpicxx")
set(CMAKE_CXX_FLAGS "-O3")

add_executable(diffusion_equation main.cpp solver.h)