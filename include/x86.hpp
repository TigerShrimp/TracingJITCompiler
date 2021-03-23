#ifndef X86_HPP
#define X86_HPP
#include "Program.hpp"

// TODO change name to Mnemonic
namespace x86 {
enum Mnemonic {
  MOV,
  ADD,
  ENTER,
  LEAVE,
  RET,
  IMUL,
  SUB,
  PUSH,
  POP,
  INC,
  MOVQ,
  ADDSD,
  MULSD,
  SUBSD,
  CMP,
  JGE,
  JMP,
  // TigerShrimp specific
  LABEL
};
}  // namespace x86
enum OpType { REGISTER, XMM_REGISTER, MEMORY, IMMEDIATE, LABEL };

enum REG {
  RAX,
  RCX,
  RDX,
  RBX,
  RSP,
  RBP,
  RSI,
  RDI,
  R8,
  R9,
  R10,
  R11,
  R12,
  R13,
  R14,
  R15
};

enum XREG { XMM0 };

struct Mem {
  REG reg;
  int offset;
};

struct Op {
  OpType opType;
  REG reg;
  XREG xreg;
  Mem mem;
  Value val;
  ProgramCounter pc;
};

bool operator<(const Op&, const Op&);
bool operator==(const Op&, const Op&);

struct Instruction {
  x86::Mnemonic inst;
  Op op1;
  Op op2;
};
#endif  // X86_HPP