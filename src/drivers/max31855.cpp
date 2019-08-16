#include <array>
#include <boost/units/systems/temperature/celsius.hpp>
#include <boost/units/systems/si.hpp>

#include "max31855.h"
#include "util/impl_bit_cast.h"

using namespace boost::units;
using namespace boost::units::si;

void Max31855::start() {
#ifdef MAX31855_SHARED
    spiAcquireBus(&this->m_spi_driver);
#endif
    spiStart(&this->m_spi_driver, &this->m_spi_config);
#ifdef MAX31855_SHARED
    spiReleaseBus(&this->m_spi_driver);
#endif

    this->state = Max31855State::Ready;
}

bool Max31855::fault_any() {
    return this->raw_spi_read().fault_any;
}
bool Max31855::fault_short_vcc() {
    return this->raw_spi_read().fault_short_vcc;
}
bool Max31855::fault_short_gnd() {
    return this->raw_spi_read().fault_short_gnd;
}
bool Max31855::fault_open() {
    return this->raw_spi_read().fault_open;
}

Max31855RawReading Max31855::raw_spi_read() {
    spiSelect(&this->m_spi_driver);
    std::array<uint8_t, 4> data = {};
    spiReceive(&this->m_spi_driver, 4, data.data());
    spiUnselect(&this->m_spi_driver);

    return bit_cast<Max31855RawReading>(data);
}

quantity<absolute<celsius::temperature>, float> demangle_temp_thermocouple(uint16_t thermocouple_temp) {
    // thermocouple_temp is a 14-bit 2's complement signed integer, packed into a uint16_t.
    // to get a int16_t out of this, we'll shift it left two bits to get the sign bit where
    // it should be, and then divide by 1<<2 = 4 to restore the magnitude.
    auto signed_temp = bit_cast<int16_t>((uint16_t)(thermocouple_temp<<2)) / 4;

    // we'll now turn it into a float and wrap it with proper units;
    // the raw thermocouple temp is given in units of 0.25°C.
    return static_cast<float>(signed_temp) * 0.25f*absolute<celsius::temperature>();
}

quantity<absolute<celsius::temperature>, float> demangle_temp_internal(uint16_t internal_temp) {
    // internal_temp is a 12-bit 2's complement signed integer, packed into a uint16_t.
    // to get a int16_t out of this, we'll shift it left four bits to get the sign bit where
    // it should be, and then divide by 1<<4 = 16 to restore the magnitude.
    auto signed_temp = bit_cast<int16_t>((uint16_t)(internal_temp<<4)) / 16;

    // we'll now turn it into a float and wrap it with proper units;
    // the internal thermocouple temp is given in units of 0.0625°C.
    return static_cast<float>(signed_temp) * 0.0625f*absolute<celsius::temperature>();
}