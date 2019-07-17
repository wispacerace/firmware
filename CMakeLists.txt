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
target_include_directories(fcu PRIVATE src/cfg)

set(ChibiOS_ROOT "${PROJECT_SOURCE_DIR}/vendor/ChibiOS")
set(ChibiOS_KERN_CONFIG_FILE "${PROJECT_SOURCE_DIR}/src/cfg/chconf.h")
set(ChibiOS_HAL_CONFIG_FILE "${PROJECT_SOURCE_DIR}/src/cfg/halconf.h")
find_package(ChibiOS)

target_link_libraries(fcu ChibiOS::RT ChibiOS::Board::NucleoF413ZH ChibiOS::HAL ChibiOS::CPPWrappers)

if(is_ipo_supported)
    set_target_properties(fcu PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CHIBIOS_MAIN_STACK_SIZE 0x400)
set(CHIBIOS_PROCESS_STACK_SIZE 0x400)
set(MCU_LINKER_SCRIPT ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/ld/STM32F413xH.ld)
set(MCU_LD_LIBRARY_PATH ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/ld)
set_target_properties(fcu PROPERTIES LINK_DEPENDS ${MCU_LINKER_SCRIPT})
target_link_options(fcu PUBLIC -nostartfiles
"-Wl,--library-path=${MCU_LD_LIBRARY_PATH},\
--script=${MCU_LINKER_SCRIPT},\
--defsym=__process_stack_size__=${CHIBIOS_PROCESS_STACK_SIZE},\
--defsym=__main_stack_size__=${CHIBIOS_MAIN_STACK_SIZE},\
--gc-sections")
target_compile_options(fcu PUBLIC -fno-exceptions -ffunction-sections -fdata-sections -fno-common
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)