#include <cstdio>
#include "ch.h"
#include "hal.h"
#include "filesystem.h"

int sd_read_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    int err = sdcRead(((lfs_bd_context*)c->context)->sdc,
        block, (uint8_t*)buffer, size/SD_BLOCK_SIZE_HC);

    return err;
}

int sd_write_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    return sdcWrite(((lfs_bd_context*)c->context)->sdc,
        block, (uint8_t*)buffer, size/SD_BLOCK_SIZE_HC);
}

int sd_erase_block(const struct lfs_config *c, lfs_block_t block) {
    return sdcErase(((lfs_bd_context*)c->context)->sdc,
        block, block+1);
}

int sd_device_sync(const struct lfs_config *c) {
    return sdcSync(((lfs_bd_context*)c->context)->sdc);
}

int FilesystemComponent::start_sdio() {
    // TODO: much nicer ways to handle errors
    sdcStart(this->sdc, &this->sdc_config);

    printf("[SDIO] Connecting... ");
    if (sdcConnect(&SDCD1)) {
        printf("failed\r\n");

        return -1;
    } else {
        printf("OK\r\n\r\nCard Info\r\n");
        static const char *mode[] = {"SDV11", "SDV20", "MMC", NULL};
        printf("CSD      : %08X %8X %08X %08X \r\n",
                    this->sdc->csd[3], this->sdc->csd[2], this->sdc->csd[1], this->sdc->csd[0]);
        printf("CID      : %08X %8X %08X %08X \r\n",
                    this->sdc->cid[3], this->sdc->cid[2], this->sdc->cid[1], this->sdc->cid[0]);
        printf("Mode     : %s\r\n", mode[this->sdc->cardmode & 3U]);
        printf("Capacity : %DMB\r\n", this->sdc->capacity / 2048);

        return 0;
    }
}

int FilesystemComponent::start() {
    int err = this->start_sdio();
    if (err) {
        printf("error starting sdio!\n");
        return err;
    }

    cfg.block_count = this->sdc->capacity/cfg.block_size;

    // mount the filesystem
    err = lfs_mount(&lfs, &cfg);
    printf("lfs_mount(): %i\n", err);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&lfs, &cfg);
        printf("lfs_format(): %i\n", err);
        err = lfs_mount(&lfs, &cfg);
        printf("lfs_mount(): %i\n", err);
    }

    return 0;
}
