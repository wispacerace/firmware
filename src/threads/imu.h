#pragma once
#include <utility>
#include <cstdio>

#include <ch.hpp>

#include "drivers/mti.h"

using namespace chibios_rt;

struct IMUMessage {
	uint16_t id;
	uint32_t timestamp;
	uint32_t status;

	float angular_velocity[3]; // TODO: replace with vector
};

struct IMUData {
    IMUMessage raw;
    float angular_velocity[3];
};

// TODO: awful, replace/abstract better
class IMUAuxThread : public BaseStaticThread<2000> {
public:
    IMUAuxThread(MtiIMU *imu, IMUSyncPipe *sync) : m_imu(imu), m_sync(sync) {}

protected:
    void main() override;

private:
    MtiIMU *m_imu;

    IMUSyncPipe *m_sync;
};

class IMUThread : public BaseStaticThread<3000> {
public:
    IMUThread(MtiIMU imu, IMUSyncPipe *sync) : m_sync(sync), m_imu(std::move(imu)) {
        chEvtObjectInit(&this->event_source);
    }

    IMUData data;
    event_source_t event_source;
protected:
    void main() override;

private:
    MtiIMU m_imu;

    IMUSyncPipe *m_sync;

};