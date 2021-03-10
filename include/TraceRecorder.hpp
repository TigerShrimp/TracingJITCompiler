#ifndef TRACERECORDER_HPP
#define TRACERECORDER_HPP
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "Program.hpp"

struct RecordEntry {
  ProgramCounter pc;
  ByteCodeInstruction inst;
};

class TraceRecorder {
 public:
  bool isRecording();
  void initRecording(ProgramCounter);
  bool record(ProgramCounter, ByteCodeInstruction);

 private:
  ProgramCounter traceStart;
  bool recording;
  std::vector<RecordEntry> recordedTrace;
};

#endif  // TRACERECORDER_HPP