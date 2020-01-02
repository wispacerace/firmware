#include <cstdio>
#include <utility>

#include "ch.hpp"
#include "hal.h"
#include "sys/fault/handlers.h"

#include "drivers/mti.h"
#include "filesystem.h"
#include "threads/sd.h"
#include "threads/imu.h"

using namespace chibios_rt;

// The watchdog timer runs off of LSI (≈32khz, the low speed clock used for
// auxiliary functions) so
//  f_lsi/(divisor * count)
//  = 32khz / (32 * 1000)
//  = 1 Hz
static WDGConfig wdg_config = {
    // divisor (IWDG_PR), clocked off of LSI
    .pr = STM32_IWDG_PR_32,
    // threshold (IWDG_RLR), the counter value that causes a chip reset
    .rlr = STM32_IWDG_RL(1000),
};

static SPIConfig spicfg_imu = {
    .circular = false,
    .end_cb = NULL,
    .ssline = LINE_SPI3_CS,
    .cr1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_CPOL | SPI_CR1_CPHA,
    .cr2 = 0,
};

static SDThread thd_sd;
static MtiIMU imu(SPID3, spicfg_imu, LINE_MTI_DRDY, LINE_MTI_RST);
static IMUSyncPipe imu_sync;
static IMUThread thd_imu(imu, &imu_sync);
static IMUAuxThread thd_imu_auxiliary(&imu, &imu_sync);
static RollControlThread thd_rollcntrl(&thd_imu.event_source, &thd_imu.data);

int main() {
    halInit();
    System::init();

    sdStart(&SD3, NULL); // serial driver 3 start default config

    printf("===== WISR FCU Booting up! =====\n");

    // start the independent watchdog timer (IWDG) built into STM32 chips.
    // this will reset the chip if it hasn't heard from our code in a while,
    // which helps us recover from crashes where our code stops executing.
    wdgStart(&WDGD1, &wdg_config);

    // thd_sd.start(NORMALPRIO - 10);
    thd_imu_auxiliary.start(NORMALPRIO-5);
    thd_imu.start(NORMALPRIO-2);
    thd_rollcntrl.start(NORMALPRIO-10);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        fflush(stdout);
        // reset the watchdog timer (let the chip know we're alive) every turn
        // of the main thread's loop
        wdgReset(&WDGD1);
        // toggle LED1 to show that the main thread is still alive
        palToggleLine(LINE_LED1);
        // wait for .5s every iteration of the main thread loop.
        // other threads will run in this time, obviously.
        chThdSleepMilliseconds(500);
    }
#pragma clang diagnostic pop
}
