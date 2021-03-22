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
  std::set<ProgramCounter> innerBranchTargets;
  std::set<ProgramCounter> outerBranchTargets;
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
  std::set<ProgramCounter> innerBranchTargets;
  std::set<ProgramCounter> outerBranchTargets;
  const Value extractParam(const JVM::Mnemonic);
  const JVM::Mnemonic extractMnemonic(const JVM::Mnemonic);
};

#endif  // TRACERECORDER_HPP