#ifndef MEMORY_HANDLER_HPP
#define MEMORY_HANDLER_HPP

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include <vector>

#include "Assembler.hpp"

typedef size_t (*pfunc)(void);

union tracePointer {
  pfunc execute;
  uint8_t *startAddr;
};

struct Memory {
  uint8_t *startAddr;
  size_t size;
};

class MemoryHandler {
 public:
  MemoryHandler();
  ~MemoryHandler();

  tracePointer writeTrace(std::vector<uint8_t>);

 private:
  size_t pageSize;
  std::vector<Memory> allocatedMemory;
  uint8_t *cursor;
  size_t currentPage;

  bool allocationNeeded(size_t);
  void allocateMemory(size_t);
};

#endif  // MEMORY_HANDLER_HPP