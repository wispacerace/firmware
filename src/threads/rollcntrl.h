#pragma once

#include <ch.hpp>
#include <hal.h>
#include <PID_v1.h>

#include "drivers/servo.h"
#include "threads/imu.h"

using namespace chibios_rt;

/* Tuning parameters */
constexpr double kp = 4;
constexpr double ki = 0.1;
constexpr double kd = 0;

static PWMConfig pwm_config = {
	10000,/* 10 kHz */
	200,
	NULL,
	{
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}
	},
	0,
	0
};

class RollControlThread : public BaseStaticThread<4800> {
public:
	RollControlThread(event_source_t *imu_evt_src, IMUData *imu_data)
		: servo1{&PWMD4, 3}, // PD15
		  rollPID{&input, &output, &setpoint, kp, ki, kd, DIRECT},
		  imu_evt_src{imu_evt_src},
		  imu_data{imu_data}
	{
		rollPID.SetOutputLimits(-90., 90.);
		rollPID.SetSampleTime(2);
		rollPID.SetMode(AUTOMATIC);
	}
protected:
	void main() override;

private:
	void null_roll();
	int init();

	double input;
	double output;
	double setpoint;

	PID rollPID;

	Servo servo1;
	// Servo servo2;
	// Servo servo3;
	// Servo servo4;
	
	// TODO: abstract
	event_source_t *imu_evt_src;
	event_listener_t imu_evt_listener;
	IMUData *imu_data;
};
