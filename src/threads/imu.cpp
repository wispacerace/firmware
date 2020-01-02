#include "imu.h"
#include "ch.h"
#include "hal.h"
#include <xsdeviceid.h>

void IMUThread::main() {
    setName("imu");

    m_imu.start();
    m_imu.wakeup(m_sync);
    auto dev_id = m_imu.read_device_id(m_sync);
    printf("[imu] found device with ID %08x\n", dev_id);
    auto subtype = XsDeviceId_getFunction(dev_id);
    printf("[imu] device is an MTi-%u %s\n", subtype, XsDeviceId_functionDescription(subtype));

    mti::OutputConfigPair mti_config[] {
        {XDI_PacketCounter, XBUS_FREQ_OUTPUT_EVERY_MESSAGE},
        {XDI_SampleTimeFine, XBUS_FREQ_OUTPUT_EVERY_MESSAGE},
        {XDI_StatusWord, XBUS_FREQ_OUTPUT_EVERY_MESSAGE},
        {XDI_RateOfTurn, 40/*hz*/},
    };
    m_imu.set_output_config(mti_config);
    // todo: parse output config

    // printf("[imu] switching to measurement state\n");
    m_imu.go_to_state(mti::DeviceState::Measurement);

    while (true) {
        palToggleLine(LINE_LED3);

        XbusParsedMessage *msg_;
        auto res = chMBFetchTimeout(&m_sync->datamsg_mbox, (msg_t*)&msg_, TIME_INFINITE);
        // TODO: get rid of this gross conversion
        if (res == MSG_OK) {
            // printf("[imu] msg\n");
            XbusMessage msg = {.mid = (enum XsMessageId) msg_->id, .length=msg_->length, msg_->payload};

            //printf("mtdata2:\n");
            if (XbusMessage_getDataItem(&this->data.raw.id, XDI_PacketCounter, &msg)) {
                //printf("  packet counter: %u\n", this->data.raw.id);
            }
            uint32_t timestamp;
            if (XbusMessage_getDataItem(&this->data.raw.timestamp, XDI_SampleTimeFine, &msg)) {
                //printf("  timestamp: %u\n", this->data.raw.timestamp);
            }
            if (XbusMessage_getDataItem(this->data.raw.angular_velocity, XDI_RateOfTurn, &msg)) {
                // printf("  ω: (%.10f, %.10f, %.10f)\n",
                //     this->data.raw.angular_velocity[0],
                //     this->data.raw.angular_velocity[1],
                //     this->data.raw.angular_velocity[2]);
            }
            if (XbusMessage_getDataItem(&this->data.raw.status, XDI_StatusWord, &msg)) {
                //printf("  status: %08x\n", this->data.raw.status);
            }

            chPoolFree(&m_sync->msgs_pool, msg_);

            for (auto i = 0; i < 3; i++)
                this->data.angular_velocity[i] = this->data.raw.angular_velocity[i];
            chEvtBroadcast(&this->event_source);
        }

        // chThdSleepMilliseconds(100);
    }
}
void IMUAuxThread::main() {
    while (true) {
        m_imu->wait_drdy();
        m_imu->handle_data(m_sync);
    }
}