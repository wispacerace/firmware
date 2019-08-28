#include <utility>

#include "ch.hpp"
#include "hal.h"
#include "chprintf.h"

#include "drivers/max31855.h"

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



static SPIConfig spicfg = {
    .circular = false,
    .end_cb = NULL,
    .ssline = LINE_SPI1_CS,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1,
    .cr2 = 0,
};

class ThermocoupleThread : public BaseStaticThread<512> {
public:
    ThermocoupleThread(Max31855 tcouple) : m_tcouple(std::move(tcouple)) {}
protected:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    void main() override {
        setName("tcouple");

        m_tcouple.start();
        while (true) {
            auto reading = m_tcouple.read();
            chprintf((BaseSequentialStream*)&SD3, "faults: any short2vcc short2gnd open\n        %d   %d        %d       %d\n",
                reading.fault_any(), reading.fault_short_vcc(), reading.fault_short_gnd(), reading.fault_open());
            auto itemp = reading.internal_temp();
            auto ttemp = reading.thermocouple_temp();

            chprintf((BaseSequentialStream*)&SD3, "internal temp is: %f\n", itemp.value());
            chprintf((BaseSequentialStream*)&SD3, "thermocouple temp is: %f\n", ttemp.value());

            palToggleLine(LINE_LED2);
            chThdSleepMilliseconds(200);
        }
    }
#pragma clang diagnostic pop

private:
    Max31855 m_tcouple;
};

static ThermocoupleThread thd_tcouple(Max31855(SPID1, spicfg));


static uint8_t sdc_scratchpad[512];
static SDCConfig sdc_config = {
    .scratchpad = sdc_scratchpad,
};

int main() {
    halInit();
    System::init();

    sdStart(&SD3, NULL); // serial driver 3 start default config

    chprintf((BaseSequentialStream*)&SD3, "===== WISR FCU Booting up! =====\n");

    // start the independent watchdog timer (IWDG) built into STM32 chips.
    // this will reset the chip if it hasn't heard from our code in a while,
    // which helps us recover from crashes where our code stops executing.
    wdgStart(&WDGD1, &wdg_config);

    sdcStart(&SDCD1, &sdc_config);

    chprintf((BaseSequentialStream*)&SD3, "[SDIO] Connecting... ");
    if (sdcConnect(&SDCD1)) {
        chprintf((BaseSequentialStream*)&SD3, "failed\r\n");
    } else {
        chprintf((BaseSequentialStream*)&SD3, "OK\r\n\r\nCard Info\r\n");
        static const char *mode[] = {"SDV11", "SDV20", "MMC", NULL};
        chprintf((BaseSequentialStream*)&SD3, "CSD      : %08X %8X %08X %08X \r\n",
                    SDCD1.csd[3], SDCD1.csd[2], SDCD1.csd[1], SDCD1.csd[0]);
        chprintf((BaseSequentialStream*)&SD3, "CID      : %08X %8X %08X %08X \r\n",
                    SDCD1.cid[3], SDCD1.cid[2], SDCD1.cid[1], SDCD1.cid[0]);
        chprintf((BaseSequentialStream*)&SD3, "Mode     : %s\r\n", mode[SDCD1.cardmode & 3U]);
        chprintf((BaseSequentialStream*)&SD3, "Capacity : %DMB\r\n", SDCD1.capacity / 2048);
    }

    init_fs();

    thd_tcouple.start(NORMALPRIO + 10);

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
