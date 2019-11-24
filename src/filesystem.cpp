#include <cstdio>
#include "ch.h"
#include "hal.h"
#include "filesystem.h"

int FilesystemComponent::start_sdio() {
    // TODO: much nicer ways to handle errors
    sdcStart(&this->sdc, &this->sdc_config);

    printf("[SDIO] Connecting... ");
    if (sdcConnect(&SDCD1)) {
        printf("failed\r\n");

        return -1;
    } else {
        printf("OK\r\n\r\nCard Info\r\n");
        static const char *mode[] = {"SDV11", "SDV20", "MMC", NULL};
        printf("CSD      : %08X %8X %08X %08X \r\n",
                    this->sdc.csd[3], this->sdc.csd[2], this->sdc.csd[1], this->sdc.csd[0]);
        printf("CID      : %08X %8X %08X %08X \r\n",
                    this->sdc.cid[3], this->sdc.cid[2], this->sdc.cid[1], this->sdc.cid[0]);
        printf("Mode     : %s\r\n", mode[this->sdc.cardmode & 3U]);
        printf("Capacity : %DMB\r\n", this->sdc.capacity / 2048);

        return 0;
    }
}

int FilesystemComponent::start() {
    int err = this->start_sdio();
    if (err) {
        printf("error starting sdio!\n");
        return err;
    }

    return 0;
}
