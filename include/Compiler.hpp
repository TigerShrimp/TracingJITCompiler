#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <list>
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
#include "utils/LRUQueue.hpp"
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
  Trace compileAndInstall(int, Recording);
  size_t bytesWritten();

 private:
  Assembler assembler;
  MemoryHandler memoryHandler;
  std::list<Instruction> nativeTrace;
  std::list<Instruction> bailoutCode;
  const ProgramCounter exitPc = {0, 0};
  const Op exitLabel = {LABEL, .pc = exitPc};
  long exitId = 0;
  std::map<long, ProgramCounter> exitPoints;
  std::stack<Op> operandStack;
  std::queue<REG> availableRegs;
  std::queue<XREG> availableXRegs;
  void resetCompilerState();
  void compile(RecordEntry, std::set<ProgramCounter>);
  void compileBailoutFor(Op);
  std::list<Instruction> generateMov(Op, Op);
  std::list<Instruction> generateVariableLoad(Op, int);
  std::list<Instruction> generateVariableStore(Op, int);
  std::list<Instruction> generateCondBranch(x86::Mnemonic, Op);
  std::list<Instruction> generateArithmetic(x86::Mnemonic);
  std::list<Instruction> generateRemDiv(bool);
  std::list<Instruction> movWithSwap(std::map<size_t, Op>&, Op, size_t);
  Op getFirstAvailableReg();
  Op popAndFree();
  Op labelAt(ProgramCounter, Value);
};
inline void concat(std::list<Instruction>&, std::list<Instruction>);

#endif  // COMPILER_HPP