set(ChibiOS_SOURCES_PORT_ARMv7M
        ${ChibiOS_ROOT}/os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v7m.S
        ${ChibiOS_ROOT}/os/common/ports/ARMCMx/chcore.c
        ${ChibiOS_ROOT}/os/common/ports/ARMCMx/chcore_v7m.c
        ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
        )

set(ChibiOS_INCLUDES_PORT_ARMv7M
        ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC
        ${ChibiOS_ROOT}/os/common/ext/ARM/CMSIS/Core/Include
        ${ChibiOS_ROOT}/os/common/ports/ARMCMx
        ${ChibiOS_ROOT}/os/common/ports/ARMCMx/compilers/GCC
        ${ChibiOS_ROOT}/os/hal/include # we need this so chcore_timer.h can find hal_st.h
        )

set(ChibiOS_INCLUDES_PORT_STM32F4
        ${ChibiOS_ROOT}/os/common/startup/ARMCMx/devices/STM32F4xx
        ${ChibiOS_ROOT}/os/common/ext/ST/STM32F4xx
        )

set(ChibiOS_SOURCES_CortexM
        ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/crt1.c
        ${ChibiOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC/vectors.S
        )

set(ChibiOS_SOURCES_RT
        ${ChibiOS_ROOT}/os/rt/src/chsys.c
        ${ChibiOS_ROOT}/os/rt/src/chdebug.c
        ${ChibiOS_ROOT}/os/rt/src/chtrace.c
        ${ChibiOS_ROOT}/os/rt/src/chvt.c
        ${ChibiOS_ROOT}/os/rt/src/chschd.c
        ${ChibiOS_ROOT}/os/rt/src/chthreads.c
        )

set(ChibiOS_SOURCES_RT_MOD_TM ${ChibiOS_ROOT}/os/rt/src/chtm.c)
set(ChibiOS_SOURCES_RT_MOD_STATISTICS ${ChibiOS_ROOT}/os/rt/src/chstats.c)
set(ChibiOS_SOURCES_RT_MOD_REGISTRY ${ChibiOS_ROOT}/os/rt/src/chregistry.c)
set(ChibiOS_SOURCES_RT_MOD_SEMAPHORES ${ChibiOS_ROOT}/os/rt/src/chsem.c)
set(ChibiOS_SOURCES_RT_MOD_MUTEXES ${ChibiOS_ROOT}/os/rt/src/chmtx.c)
set(ChibiOS_SOURCES_RT_MOD_CONDVARS ${ChibiOS_ROOT}/os/rt/src/chcond.c)
set(ChibiOS_SOURCES_RT_MOD_EVENTS ${ChibiOS_ROOT}/os/rt/src/chevents.c)
set(ChibiOS_SOURCES_RT_MOD_MESSAGES ${ChibiOS_ROOT}/os/rt/src/chmsg.c)
set(ChibiOS_SOURCES_RT_MOD_DYNAMIC ${ChibiOS_ROOT}/os/rt/src/chdynamic.c)

set(ChibiOS_SOURCES_RT_MOD_MAILBOXES ${ChibiOS_ROOT}/os/oslib/src/chmboxes.c)
set(ChibiOS_SOURCES_RT_MOD_MEMCORE ${ChibiOS_ROOT}/os/oslib/src/chmemcore.c)
set(ChibiOS_SOURCES_RT_MOD_HEAP ${ChibiOS_ROOT}/os/oslib/src/chmemheaps.c)
set(ChibiOS_SOURCES_RT_MOD_MEMPOOLS ${ChibiOS_ROOT}/os/oslib/src/chmempools.c)
set(ChibiOS_SOURCES_RT_MOD_PIPES ${ChibiOS_ROOT}/os/oslib/src/chpipes.c)
set(ChibiOS_SOURCES_RT_MOD_FACTORY ${ChibiOS_ROOT}/os/oslib/src/chfactory.c)

set(ChibiOS_INCLUDES_RT
        ${ChibiOS_ROOT}/os/rt/include
        ${ChibiOS_ROOT}/os/license
        ${ChibiOS_ROOT}/os/common/portability/GCC
        ${ChibiOS_ROOT}/os/oslib/include
        )


add_library(ChibiOS::RT INTERFACE IMPORTED)
target_sources(ChibiOS::RT INTERFACE
        ${ChibiOS_SOURCES_PORT_ARMv7M}
        ${ChibiOS_SOURCES_CortexM}
        ${ChibiOS_SOURCES_RT}
        )
target_include_directories(ChibiOS::RT INTERFACE
        ${ChibiOS_INCLUDES_RT}
        ${ChibiOS_INCLUDES_PORT_ARMv7M}
        ${ChibiOS_INCLUDES_PORT_STM32F4}
        )
# TODO: hardcoded cortex usefpu since our toolchain file sets hardfp
target_compile_definitions(ChibiOS::RT INTERFACE CORTEX_USE_FPU)

# inject modules
foreach(module ${ChibiOS_KERN_FEATURES})
    target_sources(ChibiOS::RT INTERFACE ${ChibiOS_SOURCES_RT_MOD_${module}})
endforeach()