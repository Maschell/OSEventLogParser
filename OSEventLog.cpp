#pragma once

#include <cstdio>
#include <string>
#include "OSEventLog.h"
#include "stdendian.h"
#include "StringTools.h"


uint32_t OSEventLogByteSum(const uint8_t *buffer, uint32_t len) {
   uint8_t sum = 0;
   for (uint32_t i = 0; i < len; i++) {
      sum += ((uint8_t *) buffer)[i];
   }
   return sum;
}

bool OSEventLogValidateBlockGroup(void *buffer, uint32_t startBlockId) {
   uint32_t blockCount = 1;
   uint32_t prevMagic = 0;
   auto res = true;
   for (uint32_t i = 0; i < blockCount; i++) {
      auto curBlock = OSEventBlockByID(buffer, i + startBlockId);
      auto sum = OSEventLogByteSum((uint8_t *) curBlock, sizeof(OSEventLogBlock));
      if (sum != 0xaa) {
         printf("byteSumMismatch");
         res = false;
         break;
      }
      auto val = be16(curBlock->magic);
      if (i == 0) { // first block
         if ((val & OS_EVENT_LOG_BLOCK_TYPE_MASK) != OS_EVENT_LOG_BLOCK_TYPE_1) {
            printf("Unexpected block type");
            res = false;
            break;
         }
         prevMagic = val & OS_EVENT_LOG_BLOCK_MAGIC_VALUE_MASK;
         blockCount = curBlock->firstBlock.blockCount;
      } else {
         if ((val & OS_EVENT_LOG_BLOCK_TYPE_MASK) != OS_EVENT_LOG_BLOCK_TYPE_2) {
            printf("Unexpected block type");
            res = false;
            break;
         }
         if (prevMagic != (val & OS_EVENT_LOG_BLOCK_MAGIC_VALUE_MASK)) {
            printf("magic value has changed");
            res = false;
            break;
         }
      }
   }
   return res;
}

bool OSEventLogValidateHeader(OSEventLogHeader *buffer) {
   if (be32(buffer->unknown1) != 0x4 || be32(buffer->unknown2) != 0x18 || be32(buffer->totalBlockCount) != OS_EVENT_LOG_MAX_BLOCK_COUNT) {
      printf("Invalid header %08X %08X %08X", be32(buffer->unknown1), be32(buffer->unknown2), be32(buffer->totalBlockCount));
      return false;
   }

   uint8_t sum = OSEventLogByteSum((uint8_t *) buffer, 0x14);
   if (sum != 0xAA) {
      printf("Invalid checksum");
      return false;
   }
   return true;
}


OSEventLogBlock *OSEventBlockByID(void *buffer, uint32_t blockID) {
   return (OSEventLogBlock *) ((uint8_t *) buffer + (blockID & (OS_EVENT_LOG_MAX_BLOCK_COUNT - 1)) * sizeof(OSEventLogBlock));
}

/**
 * The group is expected to be validated at this point.
 */
std::string OSEventLogBlockGroupToString(void *buffer, uint32_t startBlockId) {
   uint32_t blockCount = 1;
   std::string res;
   for (uint32_t i = 0; i < blockCount; i++) {
      auto curBlock = OSEventBlockByID(buffer, i + startBlockId);
      if (i == 0) { // first block of this log
         blockCount = curBlock->firstBlock.blockCount;
         res.append(string_format("%.*s", sizeof(curBlock->firstBlock.data), curBlock->firstBlock.data));
      } else {
         res.append(string_format("%.*s", sizeof(curBlock->otherBlock.data), curBlock->otherBlock.data));
      }
      if (i == blockCount - 1) {
         res += '\n';
      }
   }
   return res;
}