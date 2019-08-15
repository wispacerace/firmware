SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})

### ► include common gcc stm32 toolchain file
include(gcc_stm32)

### ► common flags for C and C++ compilers and the assembler
# -mthumb → emit Thumb instructions
# -mcpu=cortex-m4 → target compiler backend specifically to the cortex-m4
# -mfloat-abi=hard → all floating point operations will become FPU instructions
# -mfpu=fpv4-sp-d16 → specify available floating-point hardware on the STM32F4
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
