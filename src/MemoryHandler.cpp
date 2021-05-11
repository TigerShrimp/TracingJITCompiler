#include "MemoryHandler.hpp"

using namespace std;

MemoryHandler::MemoryHandler() {
  pageSize = sysconf(_SC_PAGE_SIZE);
  if (pageSize == -1) {
    perror("Page size error");
    exit(EXIT_FAILURE);
  }
}

void MemoryHandler::freeTraces() {
  DEBUG_PRINT("Free trace memory\n")
  for (auto mem : allocatedMemory) {
    free(mem.startAddr);
  }
  allocatedMemory.clear();
}

TracePointer MemoryHandler::writeTrace(vector<uint8_t> traceBytes) {
  // Check that there is enough allocated memory
  if (allocationNeeded(traceBytes.size())) {
    allocateMemory(traceBytes.size());
  }
  // Write the trace to memory
  uint8_t* startAddr = cursor;
  DEBUG_PRINT("Trace start address: 0x{:x}\n", (size_t)startAddr)

#ifdef TRACE_INFO
  bytesWritten += traceBytes.size();
#endif

  for (auto byte : traceBytes) {
    DEBUG_PRINT("{:02X}", byte);
    *(cursor++) = byte;
  }
  DEBUG_PRINT("\n")
  // Keep track of where the writing started,
  // connect the start address to a tracePointer and return
  TracePointer tp;
  tp.startAddr = startAddr;
  return tp;
}

size_t MemoryHandler::getBytesWritten() { return bytesWritten; }

bool MemoryHandler::allocationNeeded(size_t traceSize) {
  if (allocatedMemory.size() == 0) return true;
  Memory currentMemory = allocatedMemory[currentPage];
  size_t usedMemory = cursor - currentMemory.startAddr;
  return (currentMemory.size - usedMemory) < traceSize;
}

void MemoryHandler::allocateMemory(size_t traceSize) {
  size_t memSize = traceSize;
  // Round up so that memSize is a multiple of pageSize.
  if (memSize % pageSize) {
    memSize &= ~(pageSize - 1);
    memSize += pageSize;
  }

  // Allocate page(s)
  uint8_t* addr = (uint8_t*)aligned_alloc(pageSize, memSize);

  if (addr == NULL) {
    perror("Allocation error");
    exit(EXIT_FAILURE);
  }

  // Make executable
  if (mprotect(addr, memSize, PROT_WRITE | PROT_EXEC) == -1) {
    perror("Error when setting protection level");
    exit(EXIT_FAILURE);
  }

  // Current page += 1;
  currentPage = allocatedMemory.size();

  allocatedMemory.push_back({addr, memSize});

  // Set cursor to begining of page;
  cursor = addr;
}