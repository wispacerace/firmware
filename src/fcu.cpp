#include <utility>
#include <cstdio>

#include "ch.hpp"
#include "hal.h"
#include "sys/fault/handlers.h"

#include "drivers/max31855.h"
#include "drivers/mti.h"
#include "filesystem.h"
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



static SPIConfig spicfg_tcouple_amp = {
    .circular = false,
    .end_cb = NULL,
    .ssline = LINE_SPI1_CS,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1,
    .cr2 = 0,
};

static SPIConfig spicfg_imu = {
    .circular = false,
    .end_cb = NULL,
    .ssline = LINE_SPI3_CS,
    .cr1 = SPI_CR1_BR_1,
    .cr2 = 0,
};

class ThermocoupleThread : public BaseStaticThread<1024> {
public:
    ThermocoupleThread(Max31855 tcouple) : m_tcouple(std::move(tcouple)) {}
protected:
    void main() override {
        setName("tcouple");

        m_tcouple.start();
        while (true) {
            auto reading = m_tcouple.read();
            printf("faults: any short2vcc short2gnd open\n        %d   %d        %d       %d\n",
                reading.fault_any(), reading.fault_short_vcc(), reading.fault_short_gnd(), reading.fault_open());
            auto itemp = reading.internal_temp();
            auto ttemp = reading.thermocouple_temp();

            printf("internal temp is: %f\n", itemp.value());
            printf("thermocouple temp is: %f\n", ttemp.value());

            palToggleLine(LINE_LED2);
            chThdSleepMilliseconds(200);
        }
    }

private:
    Max31855 m_tcouple;
};


static FilesystemComponent fs(&SDCD1);
static uint8_t file_buffer[SD_BLOCK_SIZE_HC];

class SDThread : public BaseStaticThread<10000> {
public:
    SDThread() {}
protected:
    void main() override {
        setName("sd");
        if (fs.start()) {
            return;
        }


        // read current count
        uint32_t boot_count = 0;
        lfs_file_config file_cfg = {.buffer = &file_buffer};
        lfs_file_t file;
        int err = lfs_file_opencfg(&fs.lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT, &file_cfg);
        printf("lfs_file_open(): %i\n", err);
        lfs_file_read(&fs.lfs, &file, &boot_count, sizeof(boot_count));

        // update boot count
        boot_count += 1;
        lfs_file_rewind(&fs.lfs, &file);
        lfs_file_write(&fs.lfs, &file, &boot_count, sizeof(boot_count));


        // remember the storage is not updated until the file is closed successfully
        lfs_file_close(&fs.lfs, &file);

        // release any resources we were using
        lfs_unmount(&fs.lfs);

        // print the boot count
        printf("boot_count: %d\n", boot_count);
    }
};

static ThermocoupleThread thd_tcouple(Max31855(SPID1, spicfg_tcouple_amp));
static SDThread thd_sd;
static IMUThread thd_imu(MtiIMU(SPID3, spicfg_imu));

int main() {
    halInit();
    System::init();

    sdStart(&SD3, NULL); // serial driver 3 start default config

    printf("===== WISR FCU Booting up! =====\n");

    // start the independent watchdog timer (IWDG) built into STM32 chips.
    // this will reset the chip if it hasn't heard from our code in a while,
    // which helps us recover from crashes where our code stops executing.
    wdgStart(&WDGD1, &wdg_config);

    // thd_sd.start(NORMALPRIO+10);
    // thd_tcouple.start(NORMALPRIO + 10);
    thd_imu.start(NORMALPRIO+10);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
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
