#include "Profiler.hpp"

void Profiler::countVisitFor(ProgramCounter pc) {
  if (pc.methodIndex == lastPc.methodIndex &&
      pc.instructionIndex < lastPc.instructionIndex) {
    loopRecord[pc] += 1;
  }
  lastPc = pc;
}

void Profiler::countSideExitFor(ProgramCounter pc) {
  loopRecord[pc] += 1;
  lastPc = pc;
}

bool Profiler::isHot(ProgramCounter pc) {
  return loopRecord.contains(pc) && loopRecord[pc] > hotThreshold;
}