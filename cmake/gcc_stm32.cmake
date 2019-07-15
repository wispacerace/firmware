set(CMAKE_SYSTEM_NAME Generic) # 'Generic' system name is used for embedded systems
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple arm-none-eabi)

set(CMAKE_C_COMPILER   "${triple}-gcc")
set(CMAKE_CXX_COMPILER "${triple}-g++")
set(CMAKE_ASM_COMPILER "${triple}-gcc")

# don't try to link executables during internal compiler checks,
# since linking will fail without a linker script.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_OBJCOPY  "${triple}-objcopy")
set(CMAKE_OBJDUMP  "${triple}-objdump")
set(CMAKE_SIZE     "${triple}-size")
set(CMAKE_DEBUGGER "${triple}-gdb")
set(CMAKE_CPPFILT  "${triple}-c++filt")