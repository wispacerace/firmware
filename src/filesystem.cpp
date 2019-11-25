#include <cstdio>
#include "filesystem.h"

int FilesystemComponent::start_sdio() {
    // TODO: much nicer ways to handle errors
    sdcStart(&this->sdc, &this->sdc_config);

    printf("[fs/SDIO] Connecting... ");
    if (sdcConnect(&SDCD1)) {
        printf("failed\n");

        return -1;
    } else {
        printf("ok\n");
        printf("[fs/SDIO] Card Info\n");
        static const char *mode[] = {"SDV11", "SDV20", "MMC", NULL};
        printf("[fs/SDIO] CSD     : %08X %8X %08X %08X \n",
                    this->sdc.csd[3], this->sdc.csd[2], this->sdc.csd[1], this->sdc.csd[0]);
        printf("[fs/SDIO] CID     : %08X %8X %08X %08X \n",
                    this->sdc.cid[3], this->sdc.cid[2], this->sdc.cid[1], this->sdc.cid[0]);
        printf("[fs/SDIO] Mode    : %s\n", mode[this->sdc.cardmode & 3U]);
        printf("[fs/SDIO] Capacity: %DMB\n", this->sdc.capacity / 2048);

        return 0;
    }
}

int FilesystemComponent::start() {
    int err = this->start_sdio();
    if (err) {
        printf("[fs] error starting sdio!\n");
        return err;
    }

    printf("[fs] mounting filesystem... ");
    FRESULT fs_err = f_mount(&this->fat_fs, "/", 1 /* 1 = mount immediately */);
    if (fs_err == FR_OK) {
        printf("ok\n");
    } else if (fs_err == FR_NO_FILESYSTEM) {
        printf("no filesystem!\n");
        printf("[fs] creating a new exFAT filesystem... ");

        MKFS_PARM params = {
            .fmt = FM_EXFAT,
            .n_fat = 0,   // doesn't matter since this is exFAT
            .align = 0,   // automatically gets block size via fatfs_diskio.c
            .n_root = 0,  // doesn't matter since this is exFAT
            .au_size = 0, // let decide based on size
        };
        fs_err = f_mkfs("/", &params, NULL /* heap-alloc buffer (LFN==3) */, /* buffer size */ 512);
        if (fs_err != FR_OK) {
            printf("error: %i\n", fs_err);
            return fs_err;
        }
        printf("ok!\n");
    } else {
        printf("mount error: %i\n", fs_err);
        return fs_err;
    }

    return 0;
}
