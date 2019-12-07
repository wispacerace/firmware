#pragma once

#include <ch.h>
#include <hal.h>

// NOTE: this must be the only thing on the given SPIDriver
// unlike max31855, we make no effort to restart or acquire the bus,
// since sharing the IMU SPI bus with other devices would be a Bad Idea™
class MtiIMU {
public:
    MtiIMU(SPIDriver &spi_driver, SPIConfig &spi_config)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config) {}

    void start();

private:
    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;
};