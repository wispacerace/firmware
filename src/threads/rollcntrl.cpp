#include <math.h>

#include "rollcntrl.h"
#include <cstdio>


int RollControlThread::init() {
	chEvtRegister(this->imu_evt_src, &this->imu_evt_listener, 0);
    pwmStart(&PWMD4, &pwm_config);

	servo1.write(90);
//	servo2.write(90);
//	servo3.write(90);
//	servo4.write(90);

	return 0;
}

void RollControlThread::null_roll() {
	// wait for IMU data
	eventmask_t evt = chEvtWaitAny(ALL_EVENTS);

	double omegaZ = this->imu_data->angular_velocity[2];

	setpoint = 0;
	input = omegaZ;
	rollPID.Compute();
	// printf("ωz = %f, output = %f\n", omegaZ, output);
	if ((omegaZ > 0.15) || (omegaZ < -0.15)) {
		servo1.write(90 + round(output));
//		servo2.write(90 + round(output));
//		servo3.write(90 + round(output));
//		servo4.write(90 + round(output));
	} else {
		servo1.write(90);
//		servo2.write(90);
//		servo3.write(90);
//		servo4.write(90);
	}
}


void RollControlThread::main() {
	init();
	while (1) {
		null_roll();
	}
}
