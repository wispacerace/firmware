#pragma once
#include "ch.h"
#include "hal.h"

#define MAX31855_SHARED TRUE

enum class Max31855State {
    UNINIT,
    STOPPED,
    READY,
};

class Max31855 {
public:
    Max31855(SPIDriver &spi_driver, SPIConfig &spi_config)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config),
          state(Max31855State::UNINIT) {}

    void start();

private:
    uint32_t spi_read();

    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;

    Max31855State state;
};