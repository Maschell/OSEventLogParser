#pragma once

#include <string>
#include <cstdint>
#include "wut_structsize.h"

#define OS_EVENT_LOG_MAX_BLOCK_COUNT 0x00020000

struct OSEventLogBlockFirst {
    uint8_t blockCount;
    WUT_UNKNOWN_BYTES(0x01);
    uint16_t magic;
    WUT_UNKNOWN_BYTES(0x04);
    union {
        struct {
            uint8_t sessionId;
            WUT_UNKNOWN_BYTES(0x07);
        } session;
        uint64_t ticks; // unknown?
    };
    char data[0x10];
};
WUT_CHECK_OFFSET(OSEventLogBlockFirst, 0, blockCount);
WUT_CHECK_OFFSET(OSEventLogBlockFirst, 2, magic);
WUT_CHECK_OFFSET(OSEventLogBlockFirst, 8, ticks);
WUT_CHECK_OFFSET(OSEventLogBlockFirst, 8, session.sessionId);
WUT_CHECK_SIZE(OSEventLogBlockFirst, 0x20);

struct OSEventLogBlockOther {
    WUT_UNKNOWN_BYTES(0x02);
    uint16_t magic;
    char data[0x1C];
};
WUT_CHECK_SIZE(OSEventLogBlockOther, 0x20);

struct OSEventLogBlock {
    union {
        struct {
            WUT_UNKNOWN_BYTES(0x02);
            uint16_t magic;
            WUT_UNKNOWN_BYTES(0x1C);
        };
        OSEventLogBlockFirst firstBlock;
        OSEventLogBlockOther otherBlock;
    };
};
WUT_CHECK_OFFSET(OSEventLogBlock, 0x2, magic);
WUT_CHECK_SIZE(OSEventLogBlock, 0x20);


#define OS_EVENT_LOG_BLOCK_TYPE_MASK 0xC000

enum OSEventLogBlockType {
    OS_EVENT_LOG_BLOCK_TYPE_1 = 0x4000,
    OS_EVENT_LOG_BLOCK_TYPE_2 = 0x8000,
};
#define OS_EVENT_LOG_BLOCK_MAGIC_VALUE_MASK 0x3FFF


struct OSEventLogHeader {
    uint32_t unknown1;        // always expected to be 0x04
    uint32_t unknown2;        // always expected to be 0x18
    uint32_t totalBlockCount; // always expected to be 0x20000
    WUT_UNKNOWN_BYTES(0x8);
    uint32_t numberOfValidBlocks;
    WUT_PADDING_BYTES(0x08);
};
WUT_CHECK_OFFSET(OSEventLogHeader, 0x0, unknown1);
WUT_CHECK_OFFSET(OSEventLogHeader, 0x4, unknown2);
WUT_CHECK_OFFSET(OSEventLogHeader, 0x8, totalBlockCount);
WUT_CHECK_OFFSET(OSEventLogHeader, 0x14, numberOfValidBlocks);
WUT_CHECK_SIZE(OSEventLogHeader, 0x20);


uint32_t OSEventLogByteSum(const uint8_t *buffer, uint32_t len);

bool OSEventLogValidateBlockGroup(void *buffer, uint32_t startBlockId);

bool OSEventLogValidateHeader(OSEventLogHeader *buffer);

OSEventLogBlock *OSEventBlockByID(void *buffer, uint32_t blockID);

std::string OSEventLogBlockGroupToString(void *buffer, uint32_t startBlockId);