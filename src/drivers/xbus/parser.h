#pragma once

#include <cstring>
#include <cstdint>

enum class XbusParserState {
    Preamble,
    BusID,
    MessageID,
    Length,
    ExtendedLengthMSB,
    ExtendedLengthLSB,
    Payload,
    Checksum,
};

constexpr auto max_payload_size = 128;
struct XbusParsedMessage {
    uint8_t id;
    uint16_t length;
    uint8_t payload[max_payload_size];
};

size_t xbus_buf2msg(XbusParsedMessage *msg, uint8_t *buffer, size_t buf_size);
