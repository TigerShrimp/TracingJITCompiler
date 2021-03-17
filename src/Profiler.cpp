#include "Profiler.hpp"

void Profiler::note(ProgramCounter pc) {
  if (pc.methodIndex == lastPc.methodIndex &&
      pc.instructionIndex < lastPc.instructionIndex) {
    loopRecord[pc] += 1;
  }
  lastPc = pc;
}

bool Profiler::isHot(ProgramCounter pc) {
  return loopRecord.contains(pc) && loopRecord[pc] > hotThreshold;
}