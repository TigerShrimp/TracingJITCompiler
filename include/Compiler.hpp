#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <map>
#include <queue>
#include <stack>
#include <vector>

#include "Assembler.hpp"
#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "MemoryHandler.hpp"
#include "TraceHandler.hpp"
#include "TraceRecorder.hpp"
#include "x86.hpp"

// TigerShrimp compiler.
// This compiler will take a trace of JVM byte-code instructions and compile it
// to native x86_64 machinecode.
class Compiler {
 public:
  ~Compiler();
  // Initiate a compile of a trace.
  // Register allocation alg, track variables
  // Handle jumps, (fallthrough to continue on trace), maybe go through trace
  // backwards.
  // Put guards, input bailout code.
  Trace compileAndInstall(size_t, Recording);

 private:
  Assembler assembler;
  MemoryHandler memoryHandler;
  std::vector<Instruction> nativeTrace;
  std::map<size_t, Op> variableTable;
  std::map<Op, size_t> initTable;
  std::vector<Instruction> bailoutCode;
  const ProgramCounter exitPc = {0, 0};
  const Op exitLabel = {LABEL, .pc = exitPc};
  long exitId;
  std::map<long, ProgramCounter> exitPoints;
  std::stack<Op> operandStack;
  std::priority_queue<REG> availableRegs;
  std::priority_queue<XREG> availableXRegs;
  std::map<size_t, Op> initRecord;
  void resetCompilerState();
  void compile(RecordEntry, bool);
  std::vector<Instruction> restoreInitState();
  void compileBailoutFor(Op);
  std::vector<Instruction> generateMov(Op, Op);
  std::vector<Instruction> generateCondBranch(x86::Mnemonic, Op);
  std::vector<Instruction> generateArithmetic(x86::Mnemonic);
  std::vector<Instruction> movWithSwap(std::map<size_t, Op>&, Op, size_t);
  void placeInNextAvailableRegister(size_t, BaseType);
  Op getFirstAvailableReg();
  Op labelAt(ProgramCounter, Value);
};
inline void concat(std::vector<Instruction>&, std::vector<Instruction>);

#endif  // COMPILER_HPP