/*
 * This file is just to test the barometer.
 */

#include <cstdio>
#include "ch.h"
#include "hal.h"
#include "drivers/ms5611.h"

int test_barometer_main() {
	I2CConfig i2c_cfg;
	I2CDriver i2c_drv = {
		I2C_UNINIT,/* TODO: state */
		&i2c_cfg,
		0,
	};
	int32_t temp, pressure;

	ms5611 barometer = ms5611(&i2c_drv, &i2c_cfg);
	barometer.start();
	barometer.update_calculations();
	temp = barometer.get_temperature();
	pressure = barometer.get_pressure();

	printf("Temperature: %d\nPressure: %d\n", temp, pressure);

	return 0;
}
