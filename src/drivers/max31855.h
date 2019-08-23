#pragma once
#include "ch.h"
#include "hal.h"

#include <bitset>

#include <boost/units/systems/temperature/celsius.hpp>
#include <boost/units/systems/si.hpp>

using namespace boost::units;
using namespace boost::units::si;

#define MAX31855_SHARED TRUE
#define MAX31855_DEBUG FALSE

enum class Max31855State {
    Uninit,
    Stopped,
    Ready,
};

class Max31855Reading {
public:
    Max31855Reading(uint32_t raw) : inner(raw) {}

    bool fault_any() const;
    bool fault_short_vcc() const;
    bool fault_short_gnd() const;
    bool fault_open() const;

    quantity<absolute<celsius::temperature>, float> internal_temp() const;
    quantity<absolute<celsius::temperature>, float> thermocouple_temp() const;

private:
    uint32_t inner;
};

class Max31855 {
public:
    Max31855(SPIDriver &spi_driver, SPIConfig &spi_config)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config),
          state(Max31855State::Uninit) {}

    void start();

    Max31855Reading read();

private:
    uint32_t raw_spi_read();

    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;

    Max31855State state;
};
