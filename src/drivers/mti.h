#pragma once

#include <ch.h>
#include <ch.hpp>
#include <hal.h>
#include <xbusmessage.h>
#include <xbusdef.h>
#include <utility>
#include <span.h>

#include "drivers/xbus/parser.h"

using namespace chibios_rt;

constexpr auto max_xbus_message_size = 128;

struct mtssp_status {
    uint16_t notification_message_size;
    uint16_t measurement_message_size;
};


namespace mti {
    struct OutputConfigPair {
        OutputConfigPair(uint16_t data_id, uint16_t frequency) : data_id(data_id), frequency(frequency) {}
        uint16_t data_id;
        uint16_t frequency;
    };

    size_t format_output_config(uint8_t *buf, tcb::span<OutputConfigPair> cfg);
    // TODO: unit test!

    enum class DeviceState {
        Configuration,
        Measurement
    };
}


constexpr auto message_pool_size = 4;
constexpr auto data_queue_size = 2;
constexpr auto aux_queue_size = 1;
struct IMUSyncPipe {
    IMUSyncPipe() {
        chMBObjectInit(&this->datamsg_mbox, this->datamsg_queue, data_queue_size);
        chMBObjectInit(&this->auxmsg_mbox, this->auxmsg_queue, aux_queue_size);
        chPoolObjectInit(&this->msgs_pool, /* entry size = */ sizeof(XbusParsedMessage), nullptr);
        chPoolLoadArray(&this->msgs_pool, this->msgs, message_pool_size);
    }

    mailbox_t datamsg_mbox;
    mailbox_t auxmsg_mbox;
    msg_t datamsg_queue[data_queue_size] = {};
    msg_t auxmsg_queue[aux_queue_size] = {};

    uint8_t msgs[max_xbus_message_size][message_pool_size];
    memory_pool_t msgs_pool;
};

// NOTE: this must be the only thing on the given SPIDriver
// unlike max31855, we make no effort to restart or acquire the bus,
// since sharing the IMU SPI bus with other devices would be a Bad Idea™
class MtiIMU {
public:
    MtiIMU(SPIDriver &spi_driver, SPIConfig &spi_config,
           ioline_t line_DRDY, ioline_t line_RST)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config),
          m_line_DRDY(line_DRDY),
          m_line_RST(line_RST)
    {}

    void start();
    bool wakeup(IMUSyncPipe *sync);
    uint32_t read_device_id(IMUSyncPipe *sync);
    void set_output_config(tcb::span<mti::OutputConfigPair> cfg);
    void go_to_state(mti::DeviceState state);

    void wait_drdy();
    void handle_data(IMUSyncPipe *sync);
private:
    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;

    ioline_t m_line_DRDY;
    ioline_t m_line_RST;

    uint8_t m_message_buffer[max_xbus_message_size];

    void send_opcode(uint8_t opcode);
    mtssp_status read_status();
    void read_data(uint8_t pipe, uint16_t length, IMUSyncPipe *sync);

    void send_message(XbusMessage *msg);
    XbusParsedMessage* transact_message(XbusMessage *msg, IMUSyncPipe *sync);

    bool wait_for_wakeup(sysinterval_t timeout, IMUSyncPipe *sync);
};