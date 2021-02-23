cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(scheme)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-register -fPIC -march=native -Wfatal-errors")


# lib for other c++ programs
add_executable(scheme main.cc eval.cc)
