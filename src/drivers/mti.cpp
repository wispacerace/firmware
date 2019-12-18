#include <cstdio>
#include "mti.h"

void MtiIMU::start() {
    printf("[mti]: starting spi... ");
    spiStart(&this->m_spi_driver, &this->m_spi_config);
    printf("ok\n");

    palEnableLineEvent(this->m_line_DRDY, PAL_EVENT_MODE_RISING_EDGE);
}

}