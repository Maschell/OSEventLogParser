#pragma once

#include <stdint.h>
#include "wut_structsize.h"

extern "C" uint32_t OSLogRetrieve(uint32_t u1, uint8_t *buffer, uint32_t bufferSize);

struct LogRetrieveHeader {
    uint32_t totalSize;
    uint32_t numberOfEntries;
    uint32_t currentSession;
};

struct LogRetrieveData {
    WUT_PADDING_BYTES(0xC); // Header is empty for actual files
    uint32_t offsetToNext;
    uint64_t ticks;
    WUT_UNKNOWN_BYTES(0x4);
    uint8_t sessionId;
    WUT_UNKNOWN_BYTES(0x3);
    WUT_UNKNOWN_BYTES(0x4);
    char text[0]; //  dynamic length
};
WUT_CHECK_OFFSET(LogRetrieveData, 0xC, offsetToNext);
WUT_CHECK_OFFSET(LogRetrieveData, 0x10, ticks);
WUT_CHECK_OFFSET(LogRetrieveData, 0x1C, sessionId);
WUT_CHECK_OFFSET(LogRetrieveData, 0x24, text);

/**
 * // currently only works for this buffer.
 * if (OSLogRetrieve(0, reinterpret_cast<uint8_t *>(0x100a4d80), 0xc000) != 0) {
        auto *buffer = (LogHeader *) 0x100a4d80;

        DEBUG_FUNCTION_LINE_ERR("  totalSize       = %d", buffer->totalSize);
        DEBUG_FUNCTION_LINE_ERR("  numberOfEntries = %d", buffer->numberOfEntries);
        DEBUG_FUNCTION_LINE_ERR("  currentSession  = %d", buffer->currentSession);

        uint32_t totalEntries = buffer->numberOfEntries;

        FILE *file = fopen("fs:/vol/external01/test.txt", "wb");
        if (file) {
            setvbuf(file, nullptr, _IOFBF, 512 * 1024);
            uint32_t offset = 0;
            for (int i = 0; i < buffer->numberOfEntries; i++) {
                if (offset > buffer->totalSize || i > buffer->numberOfEntries) {
                    DEBUG_FUNCTION_LINE_ERR("Overflow");
                    break;
                }
                auto *cur = (LogData *) ((uint32_t) buffer + offset);
                if (cur->session != buffer->currentSession) {
                    DEBUG_FUNCTION_LINE_ERR("SKIPP!");
                    continue;
                }

                auto output = string_format("%20lld: %s", OSTicksToMilliseconds(cur->tick), cur->text);
                fwrite(output.c_str(), 1, output.length(), file);
                offset += cur->offsetToNext;
            }


            // Close file
            fclose(file);
        }
    }
 */