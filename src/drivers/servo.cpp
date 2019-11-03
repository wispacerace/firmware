#include "ch.h"
#include "hal.h"
#include "servo.h"

pwmcnt_t degreesToTicks(PWMDriver *driver, int angle) {
	/*
	 * 3600 degrees represent a whole period
	 * Conversion is servo_width = pwm_width * 3 / 2 / 20
	 */
	return PWM_FRACTION_TO_WIDTH(driver, 3600, angle * 15 / 20);
}

Servo::Servo(PWMDriver *drv, pwmchannel_t chnl) {
	this->channel = chnl;
	this->drv = drv;
	this->angle = 90;
	pwmEnableChannel(this->drv, chnl, degreesToTicks(this->drv, 90));
}

void Servo::write(int angle) {
	pwmcnt_t width;

	if (angle < 0)
		angle = 0;
	else if (angle > 180)
		angle = 180;
	this->angle = angle;
	width = degreesToTicks(this->drv, angle);
	pwmEnableChannel(this->drv, this->channel, width);
}

int Servo::read() {
	return this->angle;
}
