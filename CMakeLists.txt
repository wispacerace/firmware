cmake_minimum_required(VERSION 3.12) # ensure we're running on a modern CMake.
                                     # sorry, Ubuntu and/or Debian users! to get a recent version of CMake,
                                     # add the Kitware apt repository via the instructions at https://apt.kitware.com/

### ► set the project to build using the ARM gcc cross-compilation toolchain with Cortex-M4 specific options
set(CMAKE_TOOLCHAIN_FILE "cmake/gcc_stm32f4.cmake")

### ► configure a C, C++17, Assembly project to hold targets
project(firmware
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17) # use the C++17 standard
enable_language(ASM) # enable assembly support; used for the startup code and interrupt vector tables inside ChibiOS

### ► make sure Git submodules have been checked out, and check them out if they aren't
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

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/ChibiOS/readme.txt"
    OR NOT EXISTS "${PROJECT_SOURCE_DIR}/tools/gen-board.h/README.md")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

### ► check if IPO (link time optimization) is supported by the compiler. If the is_ipo_supported flag is true, we will turn on link-time optimization later.
include(CheckIPOSupported)
check_ipo_supported(RESULT is_ipo_supported)

### ► add the Flight Control Unit firmware as an executable target
add_executable(fcu src/fcu.cpp src/drivers/max31855.cpp)
set_target_properties(fcu PROPERTIES SUFFIX ".elf")
target_include_directories(fcu PRIVATE src/ src/cfg/)

### ► set variables pointing to ChibiOS tree and our configuration files for it, and then "find" it (load the targets that we can build it with).
set(ChibiOS_ROOT "${PROJECT_SOURCE_DIR}/vendor/ChibiOS")
set(ChibiOS_KERN_CONFIG_FILE "${PROJECT_SOURCE_DIR}/src/cfg/chconf.h")
set(ChibiOS_HAL_CONFIG_FILE "${PROJECT_SOURCE_DIR}/src/cfg/halconf.h")
find_package(ChibiOS) # this invokes cmake/FindChibiOS.cmake

### ► configure our Flight Control Unit firmware target to link against ChibiOS modules we need.
###     - ChibiOS::RT → the actual ChibiOS kernel
###     - ChibiOS::HAL → the ChibiOS hardware abstraction layer
###     - ChibiOS::CPPWrappers → C++ wrappers and runtime builtins
target_link_libraries(fcu ChibiOS::RT ChibiOS::HAL ChibiOS::CPPWrappers)

### ► find vendored Boost subset
set(VendoredBoost_ROOT "${PROJECT_SOURCE_DIR}/vendor/boost")
find_package(VendoredBoost)

### ► link FCU target against other libraries we need
target_link_libraries(fcu Boost::vendored)

### ► enable link-time optimization on the Flight Control Unit target
if(is_ipo_supported)
    set_target_properties(fcu PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

### ► generate board target and make the FCU target link to it
add_library(board-fcudev INTERFACE)
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/boards/fcudev") # make sure to create the folder that we put the output header in
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/boards/fcudev/board.h
    COMMAND python3 ${PROJECT_SOURCE_DIR}/tools/gen-board.h/gen-board-header ${PROJECT_SOURCE_DIR}/src/boards/fcudev/board.yml ${PROJECT_BINARY_DIR}/boards/fcudev/board.h
    DEPENDS ${PROJECT_SOURCE_DIR}/src/boards/fcudev/board.yml)
target_sources(board-fcudev INTERFACE
    ${PROJECT_SOURCE_DIR}/src/boards/fcudev/board.c
    ${PROJECT_BINARY_DIR}/boards/fcudev/board.h)
target_include_directories(board-fcudev INTERFACE ${PROJECT_BINARY_DIR}/boards/fcudev)

target_link_libraries(fcu board-fcudev)

### ► configure ChibiOS stack sizes and which linker script we use for the Flight Control Unit target
set(CHIBIOS_MAIN_STACK_SIZE 0x400)
set(CHIBIOS_PROCESS_STACK_SIZE 0x400)
set(MCU_LINKER_SCRIPT ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/ld/STM32F413xH.ld)
set(MCU_LD_LIBRARY_PATH ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/ld)
set_target_properties(fcu PROPERTIES LINK_DEPENDS ${MCU_LINKER_SCRIPT}) # make the FCU target depend on the linker script, so that if it changes it forces a rebuild

### ► set up link flags for the FCU target.
###    we inject two symbols here that set the main and process stack sizes in ChibiOS.
target_link_options(fcu PUBLIC -nostartfiles
"-Wl,--library-path=${MCU_LD_LIBRARY_PATH},\
--script=${MCU_LINKER_SCRIPT},\
--defsym=__process_stack_size__=${CHIBIOS_PROCESS_STACK_SIZE},\
--defsym=__main_stack_size__=${CHIBIOS_MAIN_STACK_SIZE},\
--gc-sections")

### ► set up compile options for gcc and g++.
target_compile_options(fcu PUBLIC
    -fno-exceptions # don't build in support for exceptions because they're terrible, especially on embedded systems (this is really a C++ flag but 🤷‍♀️)
    -ffunction-sections # put each function into a separate section in the output.
                        # this does two things:
                        #   1. makes link-time optimization and section garbage-collection
                        #   2. makes some of the ChibiOS linker script tricks possible
    -fdata-sections # put each data item into its own section. see comment on -ffunction-sections for rationale.
    -fno-common # do not link in the default startup code, since ChibiOS ports supplies its own
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> # if the source file under consideration is C++, add the -fno-rtti flag.
                                         # this disables RTTI, which is basically runtime type information
    )

### ► flash target for fcu
add_custom_target(flash-fcu
    DEPENDS fcu
    COMMAND openocd --file ${PROJECT_SOURCE_DIR}/dbg-scripts/flash-fcu.cfg)