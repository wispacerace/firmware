cmake_minimum_required(VERSION 3.11) # ensure we're running on a modern CMake

set(CMAKE_TOOLCHAIN_FILE "cmake/gcc_stm32f4.cmake")
project(firmware
                 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17) # use the C++17 standard

add_executable(fcu src/fcu.cpp)
