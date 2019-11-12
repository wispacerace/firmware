#include "imu.h"
#include "ch.h"
#include "hal.h"

void IMUThread::main() {
    setName("imu");

    m_imu.start();
    m_imu.test_exchange();

    while (true) {
        palToggleLine(LINE_LED3);
        chThdSleepMilliseconds(200);
    }
}