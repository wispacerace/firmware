#include <cstdio>
#include "mti.h"

void MtiIMU::start() {
    printf("[mti]: starting spi... ");
    spiStart(&this->m_spi_driver, &this->m_spi_config);
    printf("OK\n");
}

void MtiIMU::test_exchange() {
    spiSelect(&this->m_spi_driver);
    uint8_t opcode_and_padding[4] = {0x01, 'm', 't', 'i'};
    spiSend(&this->m_spi_driver, sizeof(opcode_and_padding), opcode_and_padding);
    uint8_t resp[2] = {0};
    spiReceive(&this->m_spi_driver, 2, resp);
    printf("[mti] resp: 0x%02x 0x%02x\n", resp[0], resp[1]);
    spiUnselect(&this->m_spi_driver);
}