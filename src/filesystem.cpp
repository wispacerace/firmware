#include <cstdio>
#include "ch.h"
#include "hal.h"
#include "filesystem.h"

static int sd_read_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    int err = sdcRead(&SDCD1, block, (uint8_t*)buffer, size/SD_BLOCK_SIZE_HC);

    return err;
}

static int sd_write_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    return sdcWrite(&SDCD1, block, (uint8_t*)buffer, size/SD_BLOCK_SIZE_HC);
}

static int sd_erase_block(const struct lfs_config *c, lfs_block_t block) {
    return sdcErase(&SDCD1, block, block+1);
}

static int sd_device_sync(const struct lfs_config *c) {
    return sdcSync(&SDCD1);
}

static struct lfs_config cfg = {0};
static lfs_t lfs;
static uint8_t read_buf[SD_BLOCK_SIZE_HC] __attribute__((aligned(64)));
static uint8_t write_buf[SD_BLOCK_SIZE_HC] __attribute__((aligned(64)));
static uint8_t lookahead_buf[128] __attribute__((aligned(64)));

void init_fs() {
    // TODO: assert SDC
    // TODO: make all of this a class

    cfg.read  = sd_read_block;
    cfg.prog  = sd_write_block;
    cfg.erase = sd_erase_block;
    cfg.sync  = sd_device_sync;

    cfg.read_size = SD_BLOCK_SIZE_HC;
    cfg.prog_size = SD_BLOCK_SIZE_HC;
    cfg.block_size = SD_BLOCK_SIZE_HC;
    cfg.block_count = SDCD1.capacity/SD_BLOCK_SIZE_HC;

    cfg.block_cycles = 1024;
    cfg.cache_size = SD_BLOCK_SIZE_HC;
    cfg.lookahead_size = 128;

    cfg.read_buffer = &read_buf;
    cfg.prog_buffer = &write_buf;
    cfg.lookahead_buffer = &lookahead_buf;

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);
    printf("lfs_mount(): %i\n", err);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&lfs, &cfg);
        printf("lfs_format(): %i\n", err);
        err = lfs_mount(&lfs, &cfg);
        printf("lfs_mount(): %i\n", err);
    }
}