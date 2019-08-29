#include "filesystem.h"
#include <hal.h>
#include <chprintf.h>

FATFS fs;

static uint8_t sdc_scratchpad[512];
static SDCConfig sdc_config = {
        .scratchpad = sdc_scratchpad,
};

char line[255];
FIL fil;

void fs_init() {
    chprintf((BaseSequentialStream*)&SD3, "[FS] starting [SDIO]... ");
    sdcStart(&SDCD1, &sdc_config);
    chprintf((BaseSequentialStream*)&SD3, "OK\n");

    chprintf((BaseSequentialStream*)&SD3, "[SDIO] Connecting... ");
    if (sdcConnect(&SDCD1)) {
        chprintf((BaseSequentialStream*)&SD3, "failed\n");
    } else {
        chprintf((BaseSequentialStream*)&SD3, "OK\n[SDIO] Card Info:\n");
        static const char *mode[] = {"SDV11", "SDV20", "MMC", NULL};
        chprintf((BaseSequentialStream*)&SD3, "[SDIO]   CSD      : %08X %8X %08X %08X \n",
                 SDCD1.csd[3], SDCD1.csd[2], SDCD1.csd[1], SDCD1.csd[0]);
        chprintf((BaseSequentialStream*)&SD3, "[SDIO]   CID      : %08X %8X %08X %08X \n",
                 SDCD1.cid[3], SDCD1.cid[2], SDCD1.cid[1], SDCD1.cid[0]);
        chprintf((BaseSequentialStream*)&SD3, "[SDIO]   Mode     : %s\n", mode[SDCD1.cardmode & 3U]);
        chprintf((BaseSequentialStream*)&SD3, "[SDIO]   Capacity : %DMB\n", SDCD1.capacity / 2048);
    }

    chprintf((BaseSequentialStream*)&SD3, "[FS] mounting... ");
    auto res = f_mount(&fs, "", 0);
    if (res == 0) {
        chprintf((BaseSequentialStream *) &SD3, "OK\n", res);
    } else {
        chprintf((BaseSequentialStream*)&SD3, "FAIL (result = %d)\n", res);
    }

    /* Open a text file */
    auto fr = f_open(&fil, "message.txt", FA_READ);
    chprintf((BaseSequentialStream*)&SD3, "%u", fr);
    if (!fr) {
        /* Read every line and display it */
        while (f_gets(line, sizeof line, &fil)) {
            chprintf((BaseSequentialStream*)&SD3, line);
        }

        /* Close the file */
        f_close(&fil);
    }

}