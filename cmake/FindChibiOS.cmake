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

include(ChibiOS/ChibiOS_RT)
include(ChibiOS/ChibiOS_HAL)
include(ChibiOS/ChibiOS_Boards)

add_library(ChibiOS::CPPWrappers INTERFACE IMPORTED)
target_sources(ChibiOS::CPPWrappers INTERFACE
		${ChibiOS_ROOT}/os/various/cpp_wrappers/ch.cpp
		${ChibiOS_ROOT}/os/various/cpp_wrappers/syscalls_cpp.cpp)
target_include_directories(ChibiOS::CPPWrappers INTERFACE
		${ChibiOS_ROOT}/os/various/cpp_wrappers)