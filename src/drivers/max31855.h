#pragma once
#include "ch.h"
#include "hal.h"

#define MAX31855_SHARED TRUE

enum class Max31855State {
    Uninit,
    Stopped,
    Ready,
};

struct __attribute__((packed)) Max31855RawReading {
    uint16_t thermocouple_temp : 14;
    /* ==== reserved ==== */ uint8_t _reserved1 : 1;
    bool fault_any : 1;
    uint16_t internal_temp : 12;
    /* ==== reserved ==== */ uint8_t _reserved2 : 1;
    bool fault_short_vcc : 1;
    bool fault_short_gnc : 1;
    bool fault_open : 1;
};
static_assert(sizeof(Max31855RawReading) == 4, "Max31855RawReading didn't pack to a uint32_t");

class Max31855 {
public:
    Max31855(SPIDriver &spi_driver, SPIConfig &spi_config)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config),
          state(Max31855State::Uninit) {}

    void start();

private:
    Max31855RawReading raw_spi_read();

    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;

    Max31855State state;
};