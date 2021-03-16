#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <map>
#include <queue>
#include <stack>
#include <vector>

#include "Assembler.hpp"
#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "TraceRecorder.hpp"
#include "x86.hpp"

struct CompiledTrace {
  std::map<size_t, ProgramCounter> exitPoints;
  std::vector<uint8_t> trace;
};

// TigerShrimp compiler.
// This compiler will take a trace of JVM byte-code instructions and compile it
// to native x86_64 machinecode.
class Compiler {
 public:
  // Initiate a compile of a trace.
  // Register allocation alg, track variables
  // Handle jumps, (fallthrough to continue on trace), maybe go through trace
  // backwards.
  // Put guards, input bailout code.
  CompiledTrace compile(Recording);

 private:
  Assembler assembler;
  std::vector<Instruction> nativeTrace;
  std::map<size_t, Op> variableTable;
  std::map<REG, size_t> regInitTable;
  std::stack<Op> operandStack;
  std::priority_queue<REG> availableRegs;
  std::priority_queue<XREG> availableXRegs;
  std::map<size_t, Op> initRecord;
  void resetCompilerState();
  void compile(RecordEntry, bool);
  void placeInNextAvailableRegister(size_t, BaseType);
  Op getFirstAvailableReg();
  Op labelAt(ProgramCounter, Value);
};

#endif  // COMPILER_HPP