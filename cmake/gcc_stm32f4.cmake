GET_FILENAME_COMPONENT(FIRMWARE_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
SET(CMAKE_MODULE_PATH ${FIRMWARE_CMAKE_DIR} ${CMAKE_MODULE_PATH})

# include common gcc stm32 toolchain file
include(gcc_stm32)

set(COMMON_FLAGS "-mthumb \
                  -mcpu=cortex-m4 \
                  -mfloat-abi=hard \
                  -mfpu=fpv4-sp-d16" CACHE INTERNAL "STM32F4-relevant flags common to C, CXX, ASM")

set(CMAKE_C_FLAGS   "${COMMON_FLAGS} -fno-builtin -ffunction-sections")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -fno-builtin -ffunction-sections")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler-with-cpp")

set(CMAKE_EXE_LINKER_FLAGS    "-Wl,--gc-sections ${COMMON_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${COMMON_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${COMMON_FLAGS}")
