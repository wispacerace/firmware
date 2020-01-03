#include "parser.h"
#include "xbusdef.h"
#include "xbusmessage.h"
#include <cstdio>

size_t /* TODO: use optional */ xbus_buf2msg(XbusParsedMessage *msg,
                                             uint8_t *buffer, size_t buf_size) {
    XbusParserState state;
    // reduced. full would start at Preamble.
    state = XbusParserState::MessageID;

    auto parsing = true;
    auto valid = true;
    size_t idx = 0;
    uint8_t checksum = 0xff; // assume initial busid of 0xff for reduced.

    while (parsing) {
        // printf("\tstate = ");
        // switch(state) {
        //     case XbusParserState::Preamble: printf("Preamble\n"); break;
        //     case XbusParserState::BusID: printf("BusID\n"); break;
        //     case XbusParserState::MessageID: printf("MessageID\n"); break;
        //     case XbusParserState::Length: printf("Length\n"); break;
        //     case XbusParserState::ExtendedLengthMSB:
        //     printf("ExtendedLengthMSB\n"); break; case
        //     XbusParserState::ExtendedLengthLSB:
        //     printf("ExtendedLengthLSB\n"); break; case
        //     XbusParserState::Payload: printf("Payload\n"); break; case
        //     XbusParserState::Checksum: printf("Checksum\n"); break;
        // }
        // printf("\t checksum = 0x%x\n", checksum);
        // printf("\t idx = %u\n", idx);
        // printf("\t buffer[idx] = 0x%x\n",buffer[idx]);
        // printf("\t msg.id = 0x%x\n", msg->id);
        // printf("\t msg.length = %u\n", msg->length);
        switch (state) {
        case XbusParserState::Preamble: {
            if (buffer[idx++] == XBUS_PREAMBLE) {
                checksum = 0;
                state = XbusParserState::BusID;
            }

            break;
        }
        case XbusParserState::BusID: {
            checksum += buffer[idx++];
            state = XbusParserState::MessageID;

            break;
        }
        case XbusParserState::MessageID: {
            auto byte = buffer[idx++];
            checksum += byte;
            msg->id = (enum XsMessageId)byte;
            state = XbusParserState::Length;

            break;
        }
        case XbusParserState::Length: {
            auto byte = buffer[idx++];
            checksum += byte;
            if (byte == XBUS_NO_PAYLOAD) {
                msg->length = byte;
                state = XbusParserState::Checksum;
            } else if (byte < XBUS_EXTENDED_LENGTH) {
                msg->length = byte;
                state = XbusParserState::Payload;
            } else {
                state = XbusParserState::ExtendedLengthMSB;
            }

            break;
        }
        case XbusParserState::ExtendedLengthMSB: {
            auto byte = buffer[idx++];
            checksum += byte;
            msg->length = ((uint16_t)byte) << 8;
            state = XbusParserState::ExtendedLengthLSB;

            break;
        }
        case XbusParserState::ExtendedLengthLSB: {
            auto byte = buffer[idx++];
            checksum += byte;
            msg->length |= byte;
            state = XbusParserState::Payload;

            break;
        }
        case XbusParserState::Payload: {
            if (msg->length + idx <= buf_size) {
                memcpy(msg->payload, buffer + idx, msg->length);
                for (auto j = 0; j < msg->length; j++) {
                    checksum += buffer[idx + j];
                }
                idx += msg->length;
            }
            state = XbusParserState::Checksum;

            break;
        }
        case XbusParserState::Checksum: {
            auto byte = buffer[idx++];
            checksum += byte;
            valid = (checksum == 0);
            parsing = false;
        }
        }
    }

    if (valid)
        return idx;
    else
        return -1;
}
