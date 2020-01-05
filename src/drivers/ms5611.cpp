/*
 * Driver for MS5611-01BA03 Barometric Pressure Sensor controlled
 * via SPI.
 */
 
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "ms5611.h"
 
#ifndef HAL_USE_I2C
#warning HAL_USE_I2C must be enabled to use this driver
#endif

#define PROM_ADDR(addr)	addr / MS5611_AD0

void ms5611::start() {
	uint8_t txbuf[1] = {
		MS5611_RESET
	};

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	i2cStart(this->i2c_d, this->i2c_cfg);
	/* Reset device */
	i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf, 1, NULL, 0);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif
	/* Wait at least 2.8 ms */
	chThdSleepMilliseconds(3);
}

void ms5611::calibrate() {
	uint8_t rxbuf[16];
	uint8_t txbuf[1] = {
		MS5611_READ_PROM | MS5611_AD0
	};
	uint8_t prom_addr = PROM_ADDR(MS5611_AD0);

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	do {
		i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf, 1,
		                  &rxbuf[prom_addr * 2], 2);
		this->coeff[prom_addr++] = rxbuf[1] << 8 + rxbuf[0];/* TODO: Endianness */
		txbuf[0] += MS5611_AD0;
	} while ((txbuf[0] & MS5611_ADDR_MASK) <= (MS5611_AD2 | MS5611_AD1));
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif

	/* Save PROM calibration values */
	this->prsr_sensit = rxbuf[1 * 2];/* C1 */
	this->prsr_offset = rxbuf[2 * 2];/* C2 */
	this->temp_prsens = rxbuf[3 * 2];/* C3 */
	this->temp_prsoff = rxbuf[4 * 2];/* C4 */
	this->refern_temp = rxbuf[5 * 2];/* C5 */
	this->temp_coeff = rxbuf[6 * 2];  /* C6 */
}

uint32_t ms5611::read_temp() {
	uint8_t txbuf_cnvt[1] = {
		/*
		 * TODO: I don't know what Operating System Resource (OSR)
		 * is for
		 */
		MS5611_CNVT_D2 | MS5611_OSR4096
	};

	uint8_t txbuf_read[1] = {
		MS5611_READ_ADC
	};
	uint8_t rxbuf_read[3];

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf_cnvt, 1, NULL, 0);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif

	/* TODO: Wait for conversion to complete. */

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf_read, 1, rxbuf_read,
	                  3);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif

	/* TODO: I'm not confident about the endianess */
	return rxbuf_read[2] << 16 + rxbuf_read[1] << 8 + rxbuf_read[0];
}

uint32_t ms5611::read_pressure() {
	uint8_t txbuf_cnvt[1] = {
		/*
		 * TODO: I don't know what Operating System Resource (OSR)
		 * is for
		 */
		MS5611_CNVT_D1 | MS5611_OSR4096
	};

	uint8_t txbuf_read[1] = {
		MS5611_READ_ADC
	};
	uint8_t rxbuf_read[3];

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf_cnvt, 1, NULL, 0);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif

	/* TODO: Wait for conversion to complete. */

#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(this->i2c_d);
#endif
	i2cMasterTransmit(this->i2c_d, MS5611_DEVID, txbuf_read, 1, rxbuf_read,
	                  3);
#ifdef I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(this->i2c_d);
#endif

	/* TODO: I'm not confident about the endianess */
	return rxbuf_read[2] << 16 + rxbuf_read[1] << 8 + rxbuf_read[0];
}

void ms5611::calculate(uint32_t pressure, uint32_t temp) {
	int64_t offset, offset2, sensitivity, sensitivity2;
	int32_t dT, actual_temp, temp_comps, temp2;

	/* Calculate temperature */
	dT = temp - this->prsr_offset << 8;
	actual_temp = 2000 + (dT * this->temp_coeff) >> 23;

	/* Calculate temperature compensated pressure */
	offset = this->prsr_offset << 16 + (this->temp_prsoff * dT) >> 7;
	sensitivity = this->prsr_sensit >> 15 + (this->temp_prsens * dT) >> 8;
	temp_comps = ((pressure * sensitivity) >> 21 - offset) >> 15;

	/* Second order temperature compensation */
	if (actual_temp < 2000) {/* Low temperature */
		temp2 = (dT * dT) >> 31;
		offset2 = 5 * (actual_temp - 2000) * (actual_temp - 2000) >> 1;
		sensitivity2 = 5 * (actual_temp - 2000) * (actual_temp - 2000) >> 2;

		if (actual_temp < -1500) {/* Very low temperature */
			offset2 = offset2 + 7 * (actual_temp + 1500) * (actual_temp + 1500);
			sensitivity2 = sensitivity2 + 11 * (actual_temp + 1500) * (actual_temp + 1500) >> 1;
		}
	} else {/* High temperature */
		temp2 = 0;
		offset2 = 0;
		sensitivity2 = 0;
	}

	/* Calculated pressure and temperature */
	actual_temp -= temp2;
	offset -= offset2;
	sensitivity -= sensitivity2;

	this->temperature = actual_temp;
	this->pressure = temp_comps;
}

void ms5611::update_calculations() {
	uint32_t pressure, temp;

	/* Get values from ADC */
	pressure = this->read_pressure();
	temp = this->read_temp();

	/* Calculate actual temperature and pressure */
	calculate(pressure, temp);
}

int32_t ms5611::get_temperature() {
	return this->temperature;
}

int32_t ms5611::get_pressure() {
	return this->pressure;
}
