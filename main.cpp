#include <iostream>
#include <fstream>
#include "StringTools.h"
#include "stdendian.h"
#include "OSEventLog.h"
#include <sys/stat.h>

int main(int argc, char *argv[]) {
   struct stat st{};
   if (stat("OSEventLog.bin", &st) < 0) {
      printf("\"OSEventLog.bin\" not found.");
      return -1;
   }
   std::ifstream file("OSEventLog.bin", std::ios::binary | std::ios::ate);
   std::streamsize size = file.tellg();
   file.seekg(0, std::ios::beg);

   std::vector<char> fileBuffer(size);
   if (file.read(fileBuffer.data(), size)) {
      auto *buffer = (void *) fileBuffer.data();
      uint32_t curBlockOffset = 1;
      auto *header = (OSEventLogHeader *) buffer;
      if (buffer && OSEventLogValidateHeader(header)) {
         bool result = true;
         while (true) {
            if (!OSEventLogValidateBlockGroup(buffer, curBlockOffset)) {
               result = false;
               break;
            }
            std::string log = OSEventLogBlockGroupToString(buffer, curBlockOffset);
            printf("%s", log.c_str());

            auto *curBlock = OSEventBlockByID(buffer, curBlockOffset);
            curBlockOffset += curBlock->firstBlock.blockCount;
            if (curBlockOffset > OS_EVENT_LOG_MAX_BLOCK_COUNT || curBlockOffset >= be32(header->numberOfValidBlocks)) {
               break;
            }
         }
         if (result) {
            printf("done");
         } else {
            printf("Error :(");
         }

      }
      return 0;
   }

   return 0;
}
