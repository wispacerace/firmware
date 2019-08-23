#include <array>
#include <algorithm>
#include <boost/units/systems/temperature/celsius.hpp>
#include <boost/units/systems/si.hpp>
#include "util/impl_bit_cast.h"

#include "max31855.h"
#include "chprintf.h"

using namespace boost::units;
using namespace boost::units::si;

constexpr auto BIT_FAULT_OPEN = 0;
constexpr auto BIT_FAULT_SHORT_GND = 1;
constexpr auto BIT_FAULT_SHORT_VCC = 2;
constexpr auto BIT_FAULT_ANY = 16;

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

bool Max31855Reading::fault_any() const {
    return (this->inner & (1 << BIT_FAULT_ANY)) != 0;
}
bool Max31855Reading::fault_short_vcc() const {
    return (this->inner & (1 << BIT_FAULT_SHORT_VCC)) != 0;
}
bool Max31855Reading::fault_short_gnd() const {
    return (this->inner & (1 << BIT_FAULT_SHORT_GND)) != 0;
}
bool Max31855Reading::fault_open() const {
    return (this->inner & (1 << BIT_FAULT_OPEN)) != 0;
}

quantity<absolute<celsius::temperature>, float> Max31855Reading::internal_temp() const {
#ifdef MAX31855_DEBUG
    chprintf((BaseSequentialStream*)&SD3, "[max31855] itemp extracted raw: %X from %X\n", (this->inner >> 4) & (0b111111111111), this->inner);
#endif

    return demangle_temp_internal((this->inner >> 4) & (0b111111111111));
}
quantity<absolute<celsius::temperature>, float> Max31855Reading::thermocouple_temp() const {
#ifdef MAX31855_DEBUG
    chprintf((BaseSequentialStream*)&SD3, "[max31855] ttemp extracted raw: %X from %X\n", (this->inner >> 18), this->inner);
#endif

    return demangle_temp_thermocouple(this->inner >> 18);
}

Max31855Reading Max31855::read() {
    return Max31855Reading(this->raw_spi_read());
}

uint32_t Max31855::raw_spi_read() {
#ifdef MAX31855_SHARED
    spiAcquireBus(&this->m_spi_driver);
    spiStart(&this->m_spi_driver, &this->m_spi_config);
#endif
    spiSelect(&this->m_spi_driver);
    std::array<uint8_t, 4> data = {};
    spiReceive(&this->m_spi_driver, 4, data.data());
    spiUnselect(&this->m_spi_driver);
#ifdef MAX31855_SHARED
    spiReleaseBus(&this->m_spi_driver);
#endif

#ifdef MAX31855_DEBUG
    chprintf((BaseSequentialStream*)&SD3, "[max31855] raw data: %x %x %x %x\n", data[0], data[1], data[2], data[3]);
#endif

    return data[0]<<24 | data[1] << 16 | data[2] << 8 | data[3] << 0;
}
