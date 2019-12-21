#pragma once
#include <utility>
#include <cstdio>

#include <ch.hpp>

#include "drivers/mti.h"

using namespace chibios_rt;


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

class IMUThread : public BaseStaticThread<2000> {
public:
    IMUThread(MtiIMU imu, IMUSyncPipe *sync) : m_sync(sync), m_imu(std::move(imu)) {}
protected:
    void main() override;

private:
    MtiIMU m_imu;

    IMUSyncPipe *m_sync;
};