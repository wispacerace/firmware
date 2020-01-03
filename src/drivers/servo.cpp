#include "servo.h"
#include "ch.h"
#include "hal.h"

pwmcnt_t degreesToTicks(PWMDriver *driver, int angle) {
    /* 180 is degrees spread across 900 to 2100 microseconds */
    return (PWM_FRACTION_TO_WIDTH(driver, 20000, 900) +
            PWM_FRACTION_TO_WIDTH(driver, 20000, angle * 1200 / 180));
}

Servo::Servo(PWMDriver *drv, pwmchannel_t chnl) {
    this->channel = chnl;
    this->drv = drv;
    this->angle = 90;
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

int Servo::read() { return this->angle; }
