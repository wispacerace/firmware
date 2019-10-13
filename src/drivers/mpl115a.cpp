/*
 * This is the driver for the MPL115A digital barometer.
 * This driver is written for the MPL115A2 variation, which is controlled via
 * I2C.
 */

#include "mpl115a.h"
#include "chprintf.h"

#undef DEBUG

#ifndef HAL_USE_I2C
#warning HAL_USE_I2C must be enabled to use this driver
#endif

#ifdef DEBUG
#define debug(fd, str, ...)	chprintf(fd, str, ...)
#else
#define debug(fd, str, ...)
#endif

#define S16TO32(val)	(!!(val & 0x8000) ? (0xFFFF0000 | val) : val)
#define RSHFTSEXT16(val, shft)	((!!(val & 0x8000) && !!shft) ? ((val >> shft) | (0xFFFF << (16 - shft) & 0xFFFF)) : (val >> shft))
#define RSHFTSEXT32(val, shft)	((!!(val & 0x80000000) && !!shft) ? ((val >> shft) | (0xFFFFFFFF << (32 - shft) & 0xFFFFFFFF)) : (val >> shft))
#define RSHFTSEXT64(val, shft)	(!!(val & 0x8000000000000000) ? ((val >> shft) | (0xFFFFFFFFFFFFFFFF << (64 - shft) & 0xFFFFFFFFFFFFFFFF)) : (val >> shft))

void mpl115a::start() {
	this->i2c_cfg = {
		OPMODE_I2C,
		5250,
		STD_DUTY_CYCLE,
	};
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(&this->i2c_d);
#endif
	i2cStart(&this->i2c_d, &this->i2c_cfg);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(&this->i2c_d);
#endif
}

int32_t calc_pcomp(uint16_t padc_16, uint16_t tadc_16, uint16_t a0_16,
                       uint16_t b1_16, uint16_t b2_16, uint16_t c12_16) {
	int32_t padc, tadc, a0, b1, b2, c12, pcomp;
	int64_t c12x3, b1x2, b2x2;

	/* Extend to 32 bits */
	padc = (padc_16 >> MPL115A_PADC_UNUSED) & 0x3FF;
	tadc = (tadc_16 >> MPL115A_TADC_UNUSED) & 0x3FF;
	a0 = S16TO32(RSHFTSEXT16(a0_16, MPL115A_A0_UNUSED));
	b1 = S16TO32(RSHFTSEXT16(b1_16, MPL115A_B1_UNUSED));
	b2 = S16TO32(RSHFTSEXT16(b2_16, MPL115A_B2_UNUSED));
	c12 = S16TO32(RSHFTSEXT16(c12_16, MPL115A_C12_UNUSED));

#define C12X3_SHFT	(MPL115A_C12_SHFT + MPL115A_TADC_SHFT + MPL115A_PADC_SHFT)
#define B1X2_SHFT	(MPL115A_B1_SHFT + MPL115A_PADC_SHFT)
#define B2X2_SHFT	(MPL115A_B2_SHFT + MPL115A_TADC_SHFT)

	/* Find common denominator */
#if (C12X3_SHFT <= B1X2_SHFT) && (C12X3_SHFT <= B2X2_SHFT) && (C12X3_SHFT <= MPL115A_A0_SHFT)
#define REMAIN	C12X3_SHFT
#elif	(B1X2_SHFT <= C12X3_SHFT) && (B1X2_SHFT <= B2X2_SHFT) && (B1X2_SHFT <= MPL115A_A0_SHFT)
#define REMAIN	B1X2_SHFT
#elif	(B2X2_SHFT <= C12X3_SHFT) && (B2X2_SHFT <= B1X2_SHFT) && (B2X2_SHFT <= MPL115A_A0_SHFT)
#define REMAIN	B2X2_SHFT
#else
#define REMAIN	MPL115A_A0_SHFT
#endif

	/* Find products */
	c12x3 = c12 * tadc * padc;
	c12x3 = RSHFTSEXT64(c12x3, (C12X3_SHFT - REMAIN));
	b1x2 = b1 * padc;
	b1x2 = RSHFTSEXT64(b1x2, (B1X2_SHFT - REMAIN));
	b2x2 = b2 * tadc;
	b2x2 = RSHFTSEXT64(b2x2, (B2X2_SHFT - REMAIN));

	pcomp = (a0 + c12x3 + b1x2 + b2x2) & 0xFFFFFFFF;
	return pcomp;
}

uint32_t mpl115a::read_pressure() {
	i2caddr_t addr;
	uint8_t txbuf[3];
	uint8_t rxbuf[12];
	uint16_t padc, tadc, a0, b1, b2, c12, pcomp;

	txbuf[0] = MPL115A_CONVERT;
	txbuf[1] = 0;
	txbuf[2] = MPL115A_PADC;

	/* Start conversion */
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(&this->i2c_d);
#endif
	i2cMasterTransmit(&this->i2c_d, MPL115A_DEVID, txbuf, 2, NULL, 0);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(&this->i2c_d);
#endif

	/* TODO: It takes at most 3 ms for conversion to complete */

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(&this->i2c_d);
#endif
	i2cMasterTransmit(&this->i2c_d, MPL115A_DEVID, &txbuf[3], 1, rxbuf, 12);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(&this->i2c_d);
#endif

	/* Gather data */
	padc = rxbuf[MPL115A_PADC + MPL115A_PADC] + rxbuf[MPL115A_PADC + MPL115A_PADC + 1] << 8;
	tadc = rxbuf[MPL115A_PADC + MPL115A_TADC] + rxbuf[MPL115A_PADC + MPL115A_TADC + 1] << 8;
	a0 = rxbuf[MPL115A_PADC + MPL115A_A0] + rxbuf[MPL115A_PADC + MPL115A_A0 + 1] << 8;
	b1 = rxbuf[MPL115A_PADC + MPL115A_B1] + rxbuf[MPL115A_PADC + MPL115A_B1 + 1] << 8;
	b2 = rxbuf[MPL115A_PADC + MPL115A_B2] + rxbuf[MPL115A_PADC + MPL115A_B2 + 1] << 8;
	c12 = rxbuf[MPL115A_PADC + MPL115A_C12] + rxbuf[MPL115A_PADC + MPL115A_C12 + 1] << 8;

	/*
	 * Pcomp = a0 + (b1 + c12 * Tadc) * Padc + b2 * Tadc
	 * Denominators need to be resolved
	 */
	pcomp = calc_pcomp(padc, tadc, a0, b1, b2, c12);

	/* Pressure [kPa] = pcomp * (115 - 50) / 1023 + 50 */
	return pcomp * (115 - 50) / 1023 + 50;
}
