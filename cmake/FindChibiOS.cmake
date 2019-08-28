### ► identify enabled kernel features from the chconf.h file that ChibiOS_KERN_CONFIG points at
message(STATUS "reading kernel config ${ChibiOS_KERN_CONFIG_FILE}")
file(STRINGS "${ChibiOS_KERN_CONFIG_FILE}" ChibiOS_KERN_CONFIG
        REGEX "#define +CH_CFG_USE_([a-zA-Z0-9_]+)[\t ]+TRUE")
message(STATUS "Kernel features enabled:")
foreach(linein ${ChibiOS_KERN_CONFIG})
    string(REGEX REPLACE
            "#define +CH_CFG_USE_([a-zA-Z0-9_]+) +TRUE"
            "\\1"
            FEATURE
            "${linein}")
    list(APPEND ChibiOS_KERN_FEATURES ${FEATURE})
    message(STATUS "\t[CH]: ${FEATURE}")
endforeach()


### ► identify enabled HAL peripherals from the halconf.h file that ChibiOS_HAL_CONFIG points at
message(STATUS "reading HAL config ${ChibiOS_HAL_CONFIG_FILE}")
file(STRINGS "${ChibiOS_HAL_CONFIG_FILE}" ChibiOS_HAL_CONFIG
        REGEX "#define +HAL_USE_([a-zA-Z0-9_]+)[\t ]+TRUE")
message(STATUS "HAL features enabled:")
foreach(linein ${ChibiOS_HAL_CONFIG})
    string(REGEX REPLACE
            "#define +HAL_USE_([a-zA-Z0-9_]+) +TRUE"
            "\\1"
            FEATURE
            "${linein}")
    list(APPEND ChibiOS_HAL_FEATURES ${FEATURE})
    message(STATUS "\t[HAL]: ${FEATURE}")
endforeach()

### ► include RT, HAL, and board handling scripts; these do the actual work of finding files and creating targets for them
include(ChibiOS/ChibiOS_RT)
include(ChibiOS/ChibiOS_HAL)
include(ChibiOS/ChibiOS_Boards)

### ► create one more target: the ChibiOS C++ wrappers library, which provides C++ runtime builtins and lightweight C++ wrappers over the kernel API
add_library(ChibiOS::CPPWrappers INTERFACE IMPORTED)
target_sources(ChibiOS::CPPWrappers INTERFACE
        ${ChibiOS_ROOT}/os/various/cpp_wrappers/ch.cpp
        ${ChibiOS_ROOT}/os/various/cpp_wrappers/syscalls_cpp.cpp)
target_include_directories(ChibiOS::CPPWrappers INTERFACE
        ${ChibiOS_ROOT}/os/various/cpp_wrappers)

### ► and here's one _more_ target! the fatfs interface module from ChibiOS
add_library(ChibiOS::FatFSBindings INTERFACE IMPORTED)
target_sources(ChibiOS::FatFSBindings INTERFACE
        ${ChibiOS_ROOT}/os/various/fatfs_bindings/fatfs_diskio.c
        ${ChibiOS_ROOT}/os/various/fatfs_bindings/fatfs_syscall.c)
