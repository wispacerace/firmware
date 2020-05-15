#include "servo.h"
#include "ch.h"
#include "hal.h"

pwmcnt_t degreesToTicks(const PWMDriver *driver, int angle) {
    /* 180 is degrees spread across 900 to 2100 microseconds */
    return (PWM_FRACTION_TO_WIDTH(driver, 20000, 900) +
            PWM_FRACTION_TO_WIDTH(driver, 20000, angle * 1200 / 180));
}

void Servo::write(int new_angle) {
    pwmcnt_t width;

    if (new_angle < 0)
        new_angle = 0;
    else if (new_angle > 180)
        new_angle = 180;
    this->angle = new_angle;
    width = degreesToTicks(this->drv, this->angle);
    pwmEnableChannel(this->drv, this->channel, width);
}

int Servo::read() { return this->angle; }
