#ifndef MEMORY_HANDLER_HPP
#define MEMORY_HANDLER_HPP

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include <vector>

#include "Assembler.hpp"
#include "Definitions.hpp"
#include "TraceHandler.hpp"

struct Memory {
  uint8_t *startAddr;
  size_t size;
};

class MemoryHandler {
 public:
  MemoryHandler();

  void freeTraces();
  TracePointer writeTrace(std::vector<uint8_t>);
  size_t getBytesWritten();

 private:
  size_t pageSize;
  std::vector<Memory> allocatedMemory;
  uint8_t *cursor;
  size_t currentPage;
  size_t bytesWritten = 0;

  bool allocationNeeded(size_t);
  void allocateMemory(size_t);
};

#endif  // MEMORY_HANDLER_HPP