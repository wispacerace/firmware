/*
 * Header file for MS5611-01BA03 Barometric Pressure Sensor.
 */
 #include "ch.h"
 #include "hal.h"

#define MS5611_DEVID	0x76//TODO: 7-bit value between 0x76 and 0x77

/* Commands */
#define MS5611_RESET		0x1E
#define MS5611_CNVT_D1		0x40/* Digital pressure */
#define MS5611_CNVT_D2		0x50/* Digital temperature */
#define MS5611_READ_ADC		0x0
#define MS5611_READ_PROM	0xA0

/* Command masks */
#define MS5611_OSR256		0x0
#define MS5611_OSR512		0x2
#define MS5611_OSR1024		0x4
#define MS5611_OSR2048		0x6
#define MS5611_OSR4096		0x8
#define MS5611_AD0		0x2
#define MS5611_AD1		0x4
#define MS5611_AD2		0x8
#define MS5611_ADDR_MASK	MS5611_AD2 | MS5611_AD1 | MS5611_AD0

class ms5611 {
public:
	ms5611(I2CDriver *i2c_drv, I2CConfig *i2c_cfg) :
		i2c_d(i2c_drv), i2c_cfg(i2c_cfg) {}
	void start();
	void update_calculations();
	int32_t get_temperature();
	int32_t get_pressure();
private:
	int32_t calc_temp();
	int32_t calc_tempCompPressure();
	int32_t calc_secondTempComp();
	void calculate(uint32_t pressure, uint32_t temp);
	uint32_t read_pressure();
	uint32_t read_temp();
	void calibrate();

	I2CDriver *i2c_d;
	I2CConfig *i2c_cfg;
	uint16_t coeff[6];
	uint32_t temperature;
	uint32_t pressure;
	uint16_t prsr_sensit;/* C1 */
	uint16_t prsr_offset;/* C2 */
	uint16_t temp_prsens;/* C3 */
	uint16_t temp_prsoff;/* C4 */
	uint16_t refern_temp;/* C5 */
	uint16_t temp_coeff; /* C6 */
	/*
	Read calibration data (factory calibrated) from PROM
	C1 - Pressure Sensitivity - SENS
	C2 - Pressure Offset - OFF
	C3 - Temperature coefficient of pressure sensitivity -TCS
	C4 - Temperature coefficient of pressure offset - TCO
	C5 - Reference temperature - TCO
	C6 - Temperature coefficient of the temperature - TEMPSENS
	Read digital pressure and temperature data
	D1 - Digital pressure value
	D2 - Digital temperature value

	Calculate temperature - calc_temp()
	Difference between actual and reference temperature
		dT = D2 - T = D2 - (C5 * 2^8)
	Actual temperature (-40…85°C with 0.01°C resolution)
		TEMP = 20C + dT * TEMPSENS = 2000 + dT * C6 / 2^23

	Calculate temperature compensated pressure - calc_tempCompPressure()
	Offset at actual temperature
		OFF = OFF + TCO * dT = C2 * 2^16 + (C4 * dT )/ 2^7
	Sensitivity at actual temperature
		SENS = SENS + TCS * dT = C1 * 2^15 + (C3 * dT )/ 2^8
	Temperature compensated pressure (10…1200mbar with 0.01mbar resolution)
		P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15

	SECOND ORDER TEMPERATURE COMPENSATION
	if(TEMP < 20C)

		if(TEMP < -15C)

		else

	else
		T2 = 0; OFF2 = 0; SENS2 = 0;
	*/
};
