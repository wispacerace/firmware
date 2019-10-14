set(CMAKE_SYSTEM_NAME Generic) # 'Generic' system name is used for embedded systems
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple arm-none-eabi)

if(TOOLCHAIN_PATH)
	get_filename_component(TOOLCHAIN_PATH_ABSOLUTE "${TOOLCHAIN_PATH}" REALPATH)
	set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PATH_ABSOLUTE}/${triple})
	set(PATH_PREFIX ${TOOLCHAIN_PATH_ABSOLUTE}/bin/)
endif()

if(WIN32)
	set(toolchain_binext ".exe")
endif()

set(CMAKE_C_COMPILER   "${PATH_PREFIX}${triple}-gcc${toolchain_binext}")
set(CMAKE_CXX_COMPILER "${PATH_PREFIX}${triple}-g++${toolchain_binext}")
set(CMAKE_ASM_COMPILER "${PATH_PREFIX}${triple}-gcc${toolchain_binext}")

# don't try to link executables during internal compiler checks,
# since linking will fail without a linker script.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_OBJCOPY  "${PATH_PREFIX}${triple}-objcopy${toolchain_binext}")
set(CMAKE_OBJDUMP  "${PATH_PREFIX}${triple}-objdump${toolchain_binext}")
set(CMAKE_SIZE     "${PATH_PREFIX}${triple}-size${toolchain_binext}")
set(CMAKE_DEBUGGER "${PATH_PREFIX}${triple}-gdb${toolchain_binext}")
set(CMAKE_CPPFILT  "${PATH_PREFIX}${triple}-c++filt${toolchain_binext}")