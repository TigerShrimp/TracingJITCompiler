#include "TraceRecorder.hpp"

using namespace std;
bool TraceRecorder::isRecording() { return recording; }

void TraceRecorder::initRecording(ProgramCounter pc) {
  recording = true;
  traceStart = pc;
  recordedTrace.clear();
  innerBranchTargets.clear();
  outerBranchTargets.clear();
}

Recording TraceRecorder::getRecording() {
  return {recordedTrace, innerBranchTargets, outerBranchTargets};
}

bool TraceRecorder::record(ProgramCounter pc, ByteCodeInstruction inst) {
  if (pc == traceStart && recordedTrace.size() > 0) {
    recording = false;
#ifdef DEBUG_PRINT_ON
    DEBUG_PRINT(" ---- Trace recorded: ({},{}) ----\n", traceStart.methodIndex,
                traceStart.instructionIndex);
    for (auto record : recordedTrace) {
      ByteCodeInstruction bcInst = record.inst;
      DEBUG_PRINT("{} ", JVM::byteCodeNames.at(bcInst.mnemonic));
      for (auto val : bcInst.params) {
        DEBUG_PRINT("{} ", val.toString());
      }
      DEBUG_PRINT("\n")
    }
    DEBUG_PRINT(" ---- ---------------------- ----\n")
#endif
    return true;
  }
  switch (inst.mnemonic) {
    case JVM::GOTO:
    case JVM::IFGT:
    case JVM::IF_ICMPGE:
    case JVM::IF_ICMPGT: {
      ProgramCounter branchTarget = pc;
      int offset = inst.params[0].val.intValue;
      branchTarget.instructionIndex += offset;
      // Backward branches are only present at the unconditional branch that
      // takes the pc back to the start of the loop so we treat them differently
      if (offset < 0)
        innerBranchTargets.insert(branchTarget);
      else
        outerBranchTargets.insert(branchTarget);
      break;
    }
    default:
      break;
  }
  recordedTrace.push_back({pc, inst});
  return false;
}