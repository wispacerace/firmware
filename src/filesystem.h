#pragma once
#include "ch.h"

#define SD_BLOCK_SIZE_HC 512

class FilesystemComponent {
public:
    FilesystemComponent(SDCDriver *sdc) : sdc(sdc) {
    }

    int start();

private:
    int start_sdio();

    SDCDriver &sdc;

    uint8_t sdc_scratchpad[512];
    SDCConfig sdc_config = {
        .scratchpad = sdc_scratchpad,
    };
};
