#pragma once
#include "ffconf.h"
#include <ch.h>
#include <ff.h> // FatFS, via elm-chan (an industry standard FAT driver)
#include <hal.h>

#define SD_BLOCK_SIZE_HC 512

class FilesystemComponent {
  public:
    FilesystemComponent() : sdc(FATFS_HAL_DEVICE) {}

    int start();

  private:
    int start_sdio();

    SDCDriver &sdc;

    uint8_t sdc_scratchpad[512];
    SDCConfig sdc_config = {
        .scratchpad = sdc_scratchpad,
    };

    FATFS fat_fs;
};
