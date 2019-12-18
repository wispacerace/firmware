#pragma once

#include <ch.h>
#include <ch.hpp>
#include <hal.h>
#include <xbusmessage.h>
#include <xbusdef.h>
#include <utility>

using namespace chibios_rt;


constexpr auto max_xbus_message_size = 128;
constexpr auto xbus_mq_size = 4; // # message deep queue between ISR and main functions

class MtiIMU;

class IMUMessageHandler : public BaseStaticThread<0xff> {
public:
    IMUMessageHandler(MtiIMU *imu)
        : m_imu(imu) {}
protected:
    void main() override;

private:
    MtiIMU *m_imu;
};

struct mtssp_status {
    uint16_t notification_message_size;
    uint16_t measurement_message_size;
};

// NOTE: this must be the only thing on the given SPIDriver
// unlike max31855, we make no effort to restart or acquire the bus,
// since sharing the IMU SPI bus with other devices would be a Bad Idea™
class MtiIMU {
    friend IMUMessageHandler;
public:
    MtiIMU(SPIDriver &spi_driver, SPIConfig &spi_config,
           ioline_t line_DRDY, ioline_t line_RST)
        : m_spi_driver(spi_driver),
          m_spi_config(spi_config),
          m_line_DRDY(line_DRDY),
          m_line_RST(line_RST),

          m_thd_handler(this)
    {
        chPoolObjectInit(&this->m_msgbuf_mempool, /* entry size = */ max_xbus_message_size, nullptr);
        chPoolLoadArray(&this->m_msgbuf_mempool, this->m_message_buffers, xbus_mq_size);
    }

    void start();
    bool wakeup();


private:
    SPIDriver &m_spi_driver;
    SPIConfig &m_spi_config;

    ioline_t m_line_DRDY;
    ioline_t m_line_RST;

    uint8_t m_message_buffers[xbus_mq_size][max_xbus_message_size];
    memory_pool_t m_msgbuf_mempool;

    IMUMessageHandler m_thd_handler;

    void cb_read_message();

    void send_opcode(uint8_t opcode);
    mtssp_status read_status();
    void read_data(uint8_t pipe, uint16_t length);
};
