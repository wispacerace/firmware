#include "ch.h"
#include "hal.h"

// The watchdog timer runs off of LSI (≈32khz, the low speed clock used for auxiliary functions)
// so
//  f_lsi/(divisor * count)
//  = 32khz / (32 * 1000)
//  = 1 Hz
const WDGConfig wdg_config = {
	// divisor (IWDG_PR), clocked off of LSI
	.pr = STM32_IWDG_PR_32,
	// threshold (IWDG_RLR), the counter value that causes a chip reset
	.rlr = STM32_IWDG_RL(1000),
};

int main() {
    halInit();
    chSysInit();
    
    // start the independent watchdog timer (IWDG) built into STM32 chips.
    // this will reset the chip if it hasn't heard from our code in a while, which helps us recover
    // from crashes where our code stops executing.
    wdgStart(&WDGD1, &wdg_config);

    while (true) {
    	// reset the watchdog timer (let the chip know we're alive) every turn of the main thread's loop
    	wdgReset(&WDGD1);
    	// toggle LED1 to show that the main thread is still alive
	    palToggleLine(LINE_LED1);
	    // wait for .5s every iteration of the main thread loop.
	    // other threads will run in this time, obviously.
	    chThdSleepMilliseconds(500);
    }
}
