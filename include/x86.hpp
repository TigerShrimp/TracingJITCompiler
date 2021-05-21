#ifndef X86_HPP
#define X86_HPP
#include "Program.hpp"

// TODO change name to Mnemonic
namespace x86 {
enum Mnemonic {
  MOV,
  ADD,
  SUB,
  ENTER,
  LEAVE,
  RET,
  IMUL,
  IDIV,
  PUSH,
  POP,
  INC,
  MOVQ,
  ADDSD,
  MULSD,
  SUBSD,
  CMP,
  JG,
  JGE,
  JE,
  JZ,
  JNE,
  JLE,
  JMP,
  CALL,
  // TigerShrimp specific
  LABEL
};
}  // namespace x86
enum OpType { REGISTER, XMM_REGISTER, MEMORY, IMMEDIATE, LABEL };

enum REG {
  RDI,  // argument registers
  RSI,  // argument registers
  RDX,  // argument registers
  RCX,  // argument registers
  R8,   // argument registers
  R9,   // argument registers
  R10,
  R11,
  RAX,  // Return register
  RBX,  // non-volatile
  R12,  // non-volatile
  R13,  // non-volatile
  R14,  // non-volatile
  R15,  // non-volatile
  RSP,  // Stack pointer
  RBP,  // Frame pointer
};

enum XREG {
  XMM0,
  XMM1,
  XMM2,
  XMM3,
  XMM4,
  XMM5,
  XMM6,
  XMM7,
  XMM8,
  XMM9,
  XMM10,
  XMM11,
  XMM12,
  XMM13,
  XMM14,
  XMM15
};

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