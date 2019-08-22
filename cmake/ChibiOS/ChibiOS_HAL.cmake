set(ChibiOS_INCLUDES_HAL
        ${ChibiOS_ROOT}/os/hal/include
        
        ${ChibiOS_ROOT}/os/hal/osal/rt
        )

set(ChibiOS_INCLUDES_HAL_CortexM
        ${ChibiOS_ROOT}/os/hal/ports/common/ARMCMx)

set(ChibiOS_INCLUDES_HAL_F4
        ${ChibiOS_ROOT}/os/hal/ports/STM32/STM32F4xx
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/DMAv2
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/EXTIv1
        )

set(ChibiOS_SOURCES_HAL
        ${ChibiOS_ROOT}/os/hal/src/hal.c
        ${ChibiOS_ROOT}/os/hal/src/hal_st.c
        ${ChibiOS_ROOT}/os/hal/src/hal_buffers.c
        ${ChibiOS_ROOT}/os/hal/src/hal_queues.c
        ${ChibiOS_ROOT}/os/hal/src/hal_mmcsd.c

        ${ChibiOS_ROOT}/os/hal/osal/rt/osal.c
        )

set(ChibiOS_SOURCES_HAL_F4
        ${ChibiOS_ROOT}/os/hal/ports/common/ARMCMx/nvic.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/STM32F4xx/stm32_isr.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/STM32F4xx/hal_lld.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/EXTIv1/stm32_exti.c
        )

### SUBSYSTEMS BEGIN ###
set(ChibiOS_SOURCES_HAL_SUBSYS_ADC          ${ChibiOS_ROOT}/os/hal/src/hal_adc.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_CAN          ${ChibiOS_ROOT}/os/hal/src/hal_can.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_CRY          ${ChibiOS_ROOT}/os/hal/src/hal_crypto.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_DAC          ${ChibiOS_ROOT}/os/hal/src/hal_dac.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_GPT          ${ChibiOS_ROOT}/os/hal/src/hal_gpt.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_I2C          ${ChibiOS_ROOT}/os/hal/src/hal_i2c.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_I2S          ${ChibiOS_ROOT}/os/hal/src/hal_i2s.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_ICU          ${ChibiOS_ROOT}/os/hal/src/hal_icu.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_MAC          ${ChibiOS_ROOT}/os/hal/src/hal_mac.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_MMC_SPI      ${ChibiOS_ROOT}/os/hal/src/hal_mmc_spi.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_PAL          ${ChibiOS_ROOT}/os/hal/src/hal_pal.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_PWM          ${ChibiOS_ROOT}/os/hal/src/hal_pwm.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_RTC          ${ChibiOS_ROOT}/os/hal/src/hal_rtc.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_SDC          ${ChibiOS_ROOT}/os/hal/src/hal_sdc.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_SERIAL       ${ChibiOS_ROOT}/os/hal/src/hal_serial.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_SERIAL_USB   ${ChibiOS_ROOT}/os/hal/src/hal_serial_usb.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_SIO          ${ChibiOS_ROOT}/os/hal/src/hal_sio.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_SPI          ${ChibiOS_ROOT}/os/hal/src/hal_spi.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_TRNG         ${ChibiOS_ROOT}/os/hal/src/hal_trng.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_UART         ${ChibiOS_ROOT}/os/hal/src/hal_uart.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_USB          ${ChibiOS_ROOT}/os/hal/src/hal_usb.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_WDG          ${ChibiOS_ROOT}/os/hal/src/hal_wdg.c)
set(ChibiOS_SOURCES_HAL_SUBSYS_WSPI         ${ChibiOS_ROOT}/os/hal/src/hal_wspi.c)
### SUBSYSTEMS END   ###

### LOW LEVEL DRIVERS BEGIN ###
set(ChibiOS_SOURCES_HAL_LLD_F4_ADC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_ADC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/ADCv2)

set(ChibiOS_SOURCES_HAL_LLD_F4_CAN
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_CAN
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/CANv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_CRY
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/CRYPv1/hal_crypto_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_CRY
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/CRYPv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_DAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_DAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/DACv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_GPT
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_DAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_ICU
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_DAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_PWM
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_DAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/TIMv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_PAL
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_PAL
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/GPIOv2)

## ⚠ TODO: This DOES NOT respect USE_HAL_I2C_FALLBACK and defaults to the Real Peripheral!
# This is what you want 99.9% of the time, but if anyone outside WISR ever misguidedly uses this
# set of CMake modules, heads up!
set(ChibiOS_SOURCES_HAL_LLD_F4_I2C
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/I2Cv1/hal_i2c_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_I2C
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/I2Cv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_I2C
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/I2Cv1/hal_i2c_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_I2C
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/I2Cv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_MAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/MACv1/hal_mac_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_MAC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/MACv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_USB
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_USB
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/OTGv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_WSPI
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/QUADSPIv1/hal_wspi_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_WSPI
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/QUADSPIv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_RTC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_RTC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/RTCv2)

set(ChibiOS_SOURCES_HAL_LLD_F4_SPI
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_SPI
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SPIv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_I2S
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SPIv1/hal_i2s_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_I2S
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SPIv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_SDC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SDIOV1/hal_sdc_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_SDC
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/SDIOv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_SERIAL
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/USARTv1/hal_serial_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_SERIAL
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/USARTv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_UART
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/USARTv1/hal_uart_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_UART
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/USARTv1)

set(ChibiOS_SOURCES_HAL_LLD_F4_WDG
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)
set(ChibiOS_INCLUDES_HAL_LLD_F4_WDG
        ${ChibiOS_ROOT}/os/hal/ports/STM32/LLD/xWDGv1)
### LOW LEVEL DRIVERS END   ###



add_library(ChibiOS::HAL INTERFACE IMPORTED)
target_sources(ChibiOS::HAL INTERFACE
        ${ChibiOS_SOURCES_HAL}
        ${ChibiOS_SOURCES_HAL_F4}
        )
target_include_directories(ChibiOS::HAL INTERFACE
        ${ChibiOS_INCLUDES_HAL}
        ${ChibiOS_INCLUDES_HAL_CortexM}
        ${ChibiOS_INCLUDES_HAL_F4}
        )

### ► for each enabled subsystem, inject its high-level source files into the HAL target 
foreach(subsystem ${ChibiOS_HAL_FEATURES})
    target_sources(ChibiOS::HAL INTERFACE ${ChibiOS_SOURCES_HAL_SUBSYS_${subsystem}})
endforeach()

### ► for each enabled subsystem, inject its low-level driver sources into the HAL target, and add the low-level driver as an include directory for the target. 
foreach(subsystem ${ChibiOS_HAL_FEATURES})
    target_sources(ChibiOS::HAL INTERFACE
            ${ChibiOS_SOURCES_HAL_LLD_F4_${subsystem}})
    target_include_directories(ChibiOS::HAL INTERFACE
            ${ChibiOS_INCLUDES_HAL_LLD_F4_${subsystem}})
endforeach()

### ► add ChibiOS HAL streams library target
add_library(ChibiOS::HAL::Streams INTERFACE IMPORTED)
target_sources(ChibiOS::HAL::Streams INTERFACE
    ${ChibiOS_ROOT}/os/hal/lib/streams/chprintf.c
    ${ChibiOS_ROOT}/os/hal/lib/streams/memstreams.c
    ${ChibiOS_ROOT}/os/hal/lib/streams/nullstreams.c)
target_include_directories(ChibiOS::HAL::Streams INTERFACE
    ${ChibiOS_ROOT}/os/hal/lib/streams)
target_compile_definitions(ChibiOS::HAL::Streams INTERFACE "CHPRINTF_USE_FLOAT=TRUE")