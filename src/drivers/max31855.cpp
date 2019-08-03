#include "max31855.h"
#include "util/impl_bit_cast.h"

struct Max31855RawReading {
    int16_t thermocouple;
    int16_t ref_junction;
    bool any_fault, scv_fault, scg_fault, oc_fault;
};

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

// struct __attribute__((packed)) reading {
//     int32_t thermocouple : 12,


// }

Max31855RawReading parse_reading(uint32_t data) {
    Max31855RawReading reading = {
        .thermocouple = 0,
        .ref_junction = 0,

        .any_fault = data & (1 << 16),
        .scv_fault = data & (1 << 2),
        .scg_fault = data & (1 << 1),
        .oc_fault = data & (1 << 0),
    };

    return reading;
}