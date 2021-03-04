#include "Profiler.hpp"

void Profiler::note(size_t methodIndex, size_t pc) {
  if (methodIndex == lastMethod && pc < lastPc) {
    loopRecord[methodIndex][pc] += 1;
  }
  lastPc = pc;
  lastMethod = methodIndex;
}

bool Profiler::isHot(size_t methodIndex, size_t pc) {
  return loopRecord[methodIndex][pc] > hotThreshold;
}