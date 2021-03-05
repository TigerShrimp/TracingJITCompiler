#include "Profiler.hpp"

void Profiler::note(ProgramCounter pc) {
  if (pc < lastPc) {
    loopRecord[pc] += 1;
  }
  lastPc = pc;
}

bool Profiler::isHot(ProgramCounter pc) {
  return loopRecord[pc] > hotThreshold;
}