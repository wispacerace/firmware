#include <cstdio>
#include "mti.h"

void MtiIMU::start() {
    printf("[mti]: starting spi... ");
    spiStart(&this->m_spi_driver, &this->m_spi_config);
    printf("OK\n");
}

}