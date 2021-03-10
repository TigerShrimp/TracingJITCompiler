#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <map>
#include <queue>
#include <stack>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "TraceRecorder.hpp"
#include "x86.hpp"

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
  std::vector<Instruction> compile(std::vector<RecordEntry>);

 private:
  std::vector<Instruction> nativeTrace;
  std::map<size_t, Op> variableTable;
  std::stack<Op> operandStack;
  std::priority_queue<REG> availableRegs;
  std::priority_queue<XREG> availableXRegs;
  std::map<size_t, Op> initRecord;
  void resetCompilerState();
  void compile(RecordEntry);
  void placeInNextAvailableRegister(size_t, BaseType);
  Op getFirstAvailableReg();
};

#endif  // COMPILER_HPP