#pragma once
#include <utility>

#include "ch.hpp"
#include "drivers/mti.h"

using namespace chibios_rt;


class IMUThread : public BaseStaticThread<1000> {
public:
    IMUThread(MtiIMU imu) : m_imu(std::move(imu)) {}
protected:
    void main() override;

private:
    MtiIMU m_imu;
};