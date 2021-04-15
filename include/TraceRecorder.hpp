#ifndef TRACERECORDER_HPP
#define TRACERECORDER_HPP
#include <set>
#include <sstream>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "Program.hpp"

struct RecordEntry {
  ProgramCounter pc;
  ByteCodeInstruction inst;
};

struct Recording {
  ProgramCounter startPc;
  std::vector<RecordEntry> recordedTrace;
  std::set<ProgramCounter> innerBranchTargets;
  std::set<ProgramCounter> outerBranchTargets;
};

class TraceRecorder {
 public:
  bool isRecording();
  bool recordingDone(ProgramCounter);
  void initRecording(ProgramCounter);
  void initRecording(ProgramCounter, ProgramCounter);
  Recording getRecording();
  void record(ProgramCounter, ByteCodeInstruction);

 private:
  ProgramCounter traceStart;
  ProgramCounter loopHeader;
  bool recording;
  bool lastInstructionWasBranch;
  std::vector<RecordEntry> recordedTrace;
  std::set<ProgramCounter> innerBranchTargets;
  std::set<ProgramCounter> outerBranchTargets;
  void branchFlip(ProgramCounter);
  const Value extractParam(const JVM::Mnemonic);
  const JVM::Mnemonic extractMnemonic(const JVM::Mnemonic);
  std::string constructDebugString();
};

#endif  // TRACERECORDER_HPP