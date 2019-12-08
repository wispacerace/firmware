#include <utility>
#include <cstdio>

#include "ch.hpp"
#include "hal.h"
#include "sys/fault/handlers.h"

#include "drivers/servo.h"
#include "threads/sd.h"

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

static SDThread thd_sd;

static PWMConfig pwm_config = {
    10000, // 10kHz,
    200,
    NULL,
    {
        {PWM_OUTPUT_DISABLED,    NULL},
        {PWM_OUTPUT_DISABLED,    NULL},
        {PWM_OUTPUT_DISABLED,    NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    },
    0,
    0,
};
static Servo servo(&PWMD4, 3); // PD15

int main() {
    halInit();
    System::init();

    sdStart(&SD3, NULL); // serial driver 3 start default config

    printf("===== WISR FCU Booting up! =====\n");

    // start the independent watchdog timer (IWDG) built into STM32 chips.
    // this will reset the chip if it hasn't heard from our code in a while,
    // which helps us recover from crashes where our code stops executing.
    wdgStart(&WDGD1, &wdg_config);

    thd_sd.start(NORMALPRIO-10);
    
    pwmStart(&PWMD4, &pwm_config);
    // pwmEnableChannel(&PWMD4, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));

    int angle = 0;

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
        chThdSleepMilliseconds(10);

        angle++;
        servo.write(angle);
        if (angle >= 180)
            angle = 0;
    }
#pragma clang diagnostic pop
}
