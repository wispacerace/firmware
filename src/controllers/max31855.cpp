#include "max31855.h"
#include "util/impl_bit_cast.h"

void Max31855::start() {
#ifdef MAX31855_SHARED
    spiAcquireBus(&this->m_spi_driver);
#endif
    spiStart(&this->m_spi_driver, &this->m_spi_config);
#ifdef MAX31855_SHARED
    spiReleaseBus(&this->m_spi_driver);
#endif

    this->state = Max31855State::READY;
}

uint32_t Max31855::spi_read() {
    spiSelect(&this->m_spi_driver);
    uint8_t data[4];
    spiReceive(&this->m_spi_driver, 4, data);
    spiUnselect(&this->m_spi_driver);

    return bit_cast<uint32_t>(data);
}