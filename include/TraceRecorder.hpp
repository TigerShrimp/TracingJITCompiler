#ifndef TRACERECORDER_HPP
#define TRACERECORDER_HPP
#include <set>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "Program.hpp"

struct RecordEntry {
  ProgramCounter pc;
  ByteCodeInstruction inst;
};

struct Recording {
  std::vector<RecordEntry> recordedTrace;
  std::set<ProgramCounter> branchTargets;
};

class TraceRecorder {
 public:
  bool isRecording();
  void initRecording(ProgramCounter);
  Recording getRecording();
  bool record(ProgramCounter, ByteCodeInstruction);

 private:
  ProgramCounter traceStart;
  bool recording;
  std::vector<RecordEntry> recordedTrace;
  std::set<ProgramCounter> branchTargets;
};

#endif  // TRACERECORDER_HPP