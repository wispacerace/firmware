#pragma once
#include "ch.h"

#define SD_BLOCK_SIZE_HC 512

#include "lfs.h"

int sd_read_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size);
int sd_write_block(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size);
int sd_erase_block(const struct lfs_config *c, lfs_block_t block);
int sd_device_sync(const struct lfs_config *c);

typedef struct lfs_bd_context {
    SDCDriver *sdc;
} lfs_context;

constexpr auto lookahead_size = 128;
constexpr auto block_cycles = 500;
constexpr auto cache_size = SD_BLOCK_SIZE_HC;

class FilesystemComponent {
public:
    FilesystemComponent(SDCDriver *sdc) : sdc(sdc) {
        this->bd_context.sdc = sdc;
        cfg.context = &this->bd_context;
    }

    void start();

    lfs_t lfs = {0};

private:
    int start_sdio();

    lfs_config cfg = {
        .read = sd_read_block,
        .prog = sd_write_block,
        .erase = sd_erase_block,
        .sync = sd_device_sync,

        .read_size = SD_BLOCK_SIZE_HC,
        .prog_size = SD_BLOCK_SIZE_HC,
        .block_size = SD_BLOCK_SIZE_HC,

        .block_cycles = block_cycles,
        .cache_size = cache_size,
        .lookahead_size = lookahead_size,

        .read_buffer = &read_buf,
        .prog_buffer = &write_buf,
        .lookahead_buffer = &lookahead_buf,
    };
    lfs_bd_context bd_context = {0};

    SDCDriver *sdc = nullptr;

    uint8_t read_buf[SD_BLOCK_SIZE_HC] __attribute__((aligned(64)));
    uint8_t write_buf[SD_BLOCK_SIZE_HC] __attribute__((aligned(64)));
    uint8_t lookahead_buf[lookahead_size] __attribute__((aligned(64)));

    uint8_t sdc_scratchpad[512];
    SDCConfig sdc_config = {
        .scratchpad = sdc_scratchpad,
    };
};
