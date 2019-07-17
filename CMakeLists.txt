cmake_minimum_required(VERSION 3.12) # ensure we're running on a modern CMake

set(CMAKE_TOOLCHAIN_FILE "cmake/gcc_stm32f4.cmake")

project(firmware
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17) # use the C++17 standard
enable_language(ASM)

#######################################################################################
### make sure Git submodules have been checked out, and check them out if they aren't
#######################################################################################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Updating submodules idempotently")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/ChibiOS/readme.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT is_ipo_supported)

add_executable(fcu src/fcu.cpp)
if(is_ipo_supported)
    set_target_properties(fcu PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
