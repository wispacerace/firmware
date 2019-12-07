#include "imu.h"
#include "ch.h"
#include "hal.h"

void IMUThread::main() {
    setName("imu");

    m_imu.start();

    while (true) {
        palToggleLine(LINE_LED3);
        chThdSleepMilliseconds(200);
    }
}