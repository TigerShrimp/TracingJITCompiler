#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <asmjit/x86.h>

#include <vector>

struct Instruction {
  asmjit::x86::Inst::Id inst;
  asmjit::x86::Operand op1;
  asmjit::x86::Operand op2;
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
  std::vector<Instruction> compile(std::vector<uint8_t>);
};

#endif  // COMPILER_HPP