#include "mti.h"
#include <cassert>
#include <cstdio>

void MtiIMU::handle_data(IMUSyncPipe *sync) {
    spiSelect(&this->m_spi_driver);
    this->send_opcode(XBUS_PIPE_STATUS);

    mtssp_status status = this->read_status();
    spiUnselect(&this->m_spi_driver);

    if (status.notification_message_size > 0) {
        // printf("processing notifications\n");
        this->read_data(XBUS_NOTIFICATION_PIPE,
                        status.notification_message_size, sync);
    } else if (status.measurement_message_size > 0) {
        // printf("processing measurements\n");
        this->read_data(XBUS_MEASUREMENT_PIPE, status.measurement_message_size,
                        sync);
    }
}

void MtiIMU::read_data(uint8_t pipe, uint16_t length, IMUSyncPipe *sync) {
    assert(length <= max_xbus_message_size);

    spiSelect(&this->m_spi_driver);
    this->send_opcode(pipe); // select the given pipe
    spiReceive(&this->m_spi_driver, length,
               this->m_message_buffer); // and drain (length) bytes out of it
    spiUnselect(&this->m_spi_driver);

    // grab a message
    auto idx = 0;
    while (idx < length) {
        auto msg = (XbusParsedMessage *)chPoolAlloc(&sync->msgs_pool);
        if (msg != nullptr) {
            auto parsed_length =
                xbus_buf2msg(msg, this->m_message_buffer + idx, length - idx);
            if (parsed_length != -1) {
                idx += parsed_length;
                // printf("!\n");

                if (msg->id == XMID_MtData2) {
                    chMBPostTimeout(&sync->datamsg_mbox, (msg_t)msg,
                                    TIME_IMMEDIATE);
                } else {
                    chMBPostTimeout(&sync->auxmsg_mbox, (msg_t)msg,
                                    TIME_IMMEDIATE);
                }
            } else {
                chPoolFree(&sync->msgs_pool, msg);
                break;
            }
        }
    }
}

void MtiIMU::start() {
    printf("[mti]: starting spi... ");
    spiStart(&this->m_spi_driver, &this->m_spi_config);
    printf("ok\n");

    palEnableLineEvent(this->m_line_DRDY, PAL_EVENT_MODE_RISING_EDGE);
}

bool MtiIMU::wakeup(IMUSyncPipe *sync) {
    // bounce the reset line to make sure the IMU is in a clean state
    palSetLine(this->m_line_RST);
    chThdSleepMilliseconds(10);
    palClearLine(this->m_line_RST); // release reset line

    printf("[mti]: reset... waiting for wakeup packet\n");
    if (wait_for_wakeup(TIME_MS2I(1000), sync)) {
        printf("[mti]: sending wakeup ack... ");

        XbusMessage msg = {.mid = XMID_WakeupAck};
        this->send_message(&msg);
        printf("ok\n");
    }

    return false;
}

uint32_t MtiIMU::read_device_id(IMUSyncPipe *sync) {
    XbusMessage tx = {.mid = XMID_ReqDid};
    auto rx = this->transact_message(&tx, sync);

    if (rx != nullptr) {
        if (rx->id == XMID_DeviceId) {
            uint32_t id = (uint32_t)rx->payload[0] << 24 |
                          (uint32_t)rx->payload[1] << 16 |
                          (uint32_t)rx->payload[2] << 8 |
                          (uint32_t)rx->payload[3] << 0;
            chPoolFree(&sync->msgs_pool, rx);
            return id;
        }
        chPoolFree(&sync->msgs_pool, rx);
    }

    return 0;
}

void MtiIMU::set_output_config(tcb::span<mti::OutputConfigPair> cfg) {
    uint8_t data[32]; // TODO: should we be bounds checking this?

    auto payload_length = mti::format_output_config(data, cfg);
    XbusMessage msg = {.mid = XMID_SetOutputConfig,
                       .length = (uint16_t)payload_length,
                       .data = data};
    this->send_message(&msg);
}

void MtiIMU::go_to_state(mti::DeviceState state) {
    XbusMessage msg = {};
    switch (state) {
    case mti::DeviceState::Configuration:
        msg.mid = XMID_GotoConfig;
        break;
    case mti::DeviceState::Measurement:
        msg.mid = XMID_GotoMeasurement;
        break;
    }

    this->send_message(&msg);
}

void MtiIMU::send_opcode(uint8_t opcode) {
    uint8_t data[4] = {opcode, 0x0, 0x0, 0x0};
    spiSend(&this->m_spi_driver, sizeof(data), data);
}

mtssp_status MtiIMU::read_status() {
    uint8_t status_data[4];
    spiReceive(&this->m_spi_driver, sizeof(status_data), status_data);

    mtssp_status status = {
        .notification_message_size =
            (uint16_t)(status_data[0] | (status_data[1] << 8)),
        .measurement_message_size =
            (uint16_t)(status_data[2] | (status_data[3] << 8)),
    };

    return status;
}

void MtiIMU::wait_drdy() {
    palWaitLineTimeout(this->m_line_DRDY, TIME_INFINITE);
}

bool MtiIMU::wait_for_wakeup(sysinterval_t timeout, IMUSyncPipe *sync) {
    XbusParsedMessage *msg;
    auto res = chMBFetchTimeout(&sync->auxmsg_mbox, (msg_t *)&msg, timeout);
    if (res == MSG_OK) {
        auto id = msg->id;
        chPoolFree(&sync->msgs_pool, msg);
        if (id == XMID_Wakeup) {
            printf("[mti]: got wakeup packet\n");
            return true;
        }
    }
    return false;
}

void MtiIMU::send_message(XbusMessage *msg) {
    uint8_t buf[64]; // TODO: should we be bounds checking this?
    size_t length = XbusMessage_format(buf, msg, XLLF_Spi);
    // __asm__("bkpt");
    spiSelect(&this->m_spi_driver);
    spiSend(&this->m_spi_driver, length, buf);
    spiUnselect(&this->m_spi_driver);
}

XbusParsedMessage *MtiIMU::transact_message(XbusMessage *tx,
                                            IMUSyncPipe *sync) {
    this->send_message(tx);

    XbusParsedMessage *rx;
    auto res =
        chMBFetchTimeout(&sync->auxmsg_mbox, (msg_t *)&rx, TIME_MS2I(500));
    return (res == MSG_OK) ? rx : nullptr;
}

size_t mti::format_output_config(uint8_t *buf,
                                 tcb::span<OutputConfigPair> cfg) {
    size_t idx = 0;
    for (const auto &pair : cfg) {
        buf[idx++] = (pair.data_id >> 8) & 0xff;
        buf[idx++] = pair.data_id & 0xff;
        buf[idx++] = (pair.frequency >> 8) & 0xff;
        buf[idx++] = pair.frequency & 0xff;
    }
    return idx;
}
