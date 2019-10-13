/*
 * This is the header file for the MPL115A device driver.
 */
#include "ch.h"
#include "hal.h"
#include "boost/units/systems/si.hpp"

#define MPL115A_DEVID	0x60

/* MPL115A register addresses. Data is big endian. */
#define MPL115A_PADC	0x00
#define MPL115A_TADC	0x02
#define MPL115A_A0	0x04
#define MPL115A_B1	0x06
#define MPL115A_B2	0x08
#define MPL115A_C12	0x0A
#define MPL115A_CONVERT	0x12

/*
 * All registers are binary, but vary in the location of its decimal point.
 * Unused bits are the least significant bits. The location of the decimal
 * point, and the number of unused bits affect the value of the register.
 */

/* uint10_t */
#define MPL115A_PADC_SHFT	0
#define MPL115A_PADC_UNUSED	6

/* uint10_t */
#define MPL115A_TADC_SHFT	0
#define MPL115A_TADC_UNUSED	6

/* SIIIIIII IIIII.FFF */
#define MPL115A_A0_SHFT		3
#define MPL115A_A0_UNUSED	0

/* SII.FFFFF FFFFFFFF */
#define MPL115A_B1_SHFT		13
#define MPL115A_B1_UNUSED	0

/* SI.FFFFFF FFFFFFFF */
#define MPL115A_B2_SHFT		14
#define MPL115A_B2_UNUSED	0

/* S 0.000000000 FFFFF FFFFFFFF */
#define MPL115A_C12_SHFT	22
#define MPL115A_C12_UNUSED	2

/* 
 * Pcomp = a0 + (b1 + c12 * Tadc) * Padc + b2 * Tadc
 * Pressure [kPa] = Pcomp * (115 - 50) / 1023 + 50
 */

class mpl115a {
public:
	mpl115a();
	void start();
	uint32_t read_pressure();
private:
	I2CDriver i2c_d;
	I2CConfig i2c_cfg;
	int32_t calc_pcomp(uint16_t padc_16, uint16_t tadc_16, uint16_t a0_16, uint16_t b1_16, uint16_t b2_16, uint16_t c12_16);
};
