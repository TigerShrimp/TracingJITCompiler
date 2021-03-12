#ifndef X86_HPP
#define X86_HPP
#include "Program.hpp"

namespace x86 {
enum Inst {
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

enum REG { RAX };

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

struct Instruction {
  x86::Inst inst;
  Op op1;
  Op op2;
};
#endif  // X86_HPP