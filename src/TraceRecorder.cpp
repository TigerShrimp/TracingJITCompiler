#include "TraceRecorder.hpp"

using namespace std;
bool TraceRecorder::isRecording() { return recording; }

void TraceRecorder::initRecording(ProgramCounter pc) {
  recording = true;
  traceStart = pc;
  recordedTrace.clear();
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
  recordedTrace.push_back({pc, inst});
  return true;
}