#ifndef HEXHELPER_HPP
#define HEXHELPER_HPP
#include <regex>
#include <string>
#include <vector>

typedef unsigned char hexCode;

enum OpType { Reg, Mem, Imm, None };

enum Mode { Low, High };

struct Operand {
  OpType type;
  int val;
  Mode mode;
};

enum Mnemonic {
  MOV,
  ADD,
  // etc...
};

struct Instruction {
  Mnemonic mnem;
  Operand op1;
  Operand op2;
};

enum Register : hexCode {
  RAX = 0xC0,
  RCX = 0xC8,
  RDX = 0xD0,
  RBX = 0xD8,
  RSP = 0xE0,
  RBP = 0xE8,
  RSI = 0xF0,
  RDI = 0xF8,
  R8 = 0xC0,
  R9 = 0xC8,
  R10 = 0xD0,
  R11 = 0xD8,
  R12 = 0xE0,
  R13 = 0xE8,
  R14 = 0xF0,
  R15 = 0xF8
};

namespace Hex {

#define OP_SIZE_64 0x48

#define MOV_RM_I 0xC7
#define MOV_RM_R 0x88
#define MOV_R_RM 0x89

static const std::regex regReg(
    "RDI|RSI|RDX|RCX|R8|R9|R10|R11|RAX|RBX|R12|R13|R14|R15");
static const std::regex lowRegReg("RDI|RSI|RDX|RCX|RAX|RBX|RSP|RBP");
static const std::regex memReg("\\[RBP\\+[0-9]+\\]");
static const std::regex immReg("^[0-9]+$");

const hexCode calcHex(Operand op) {}

const hexCode getMode(Instruction instruction) {
  hexCode mode = 0;
  if (instruction.op1.type == Mem) {
    if (instruction.op2.mode == High) {
      return 0x4c;
    }
    return 0x49;
  } else if (instruction.op2.type == Mem) {
    if (instruction.op1.mode == High) {
      return 0x4c;
    }
    return 0x49;
  } else if (instruction.op1.type == Reg && instruction.op1.mode == Low) {
    mode = 0x48;
  } else if (instruction.op1.type == Reg && instruction.op1.mode == High) {
    mode = 0x49;
  }
  if (instruction.op2.type == Reg && instruction.op2.mode == High) {
    mode += 5;
  }
  return mode;
}

const Mode getMode(std::string opString) {
  return regex_match(opString.begin(), opString.end(), lowRegReg) ? Low : High;
}

const OpType getOpType(std::string operand) {
  OpType op;
  if (regex_match(operand.begin(), operand.end(), immReg)) {
    op = Imm;
  } else if (regex_match(operand.begin(), operand.end(), memReg)) {
    op = Mem;
  } else if (regex_match(operand.begin(), operand.end(), regReg)) {
    op = Reg;
  } else {
    throw;
  }
  return op;
}

const int getOpVal(OpType type, std::string opString) {
  switch (type) {
    case Imm:
      return std::stoi(opString);
    case Mem:
      // Japp
      break;
    case Reg:
      if (opString == "RAX") {
        return Register::RAX;
      } else if (opString == "RCX") {
        return Register::RCX;
      } else if (opString == "RDX") {
        return Register::RDX;
      } else if (opString == "RBX") {
        return Register::RBX;
      } else if (opString == "RSP") {
        return Register::RSP;
      } else if (opString == "RBP") {
        return Register::RBP;
      } else if (opString == "RSI") {
        return Register::RSI;
      } else if (opString == "RDI") {
        return Register::RDI;
      } else if (opString == "R8") {
        return Register::R8;
      } else if (opString == "R9") {
        return Register::R9;
      } else if (opString == "R10") {
        return Register::R10;
      } else if (opString == "R11") {
        return Register::R11;
      } else if (opString == "R12") {
        return Register::R12;
      } else if (opString == "R13") {
        return Register::R13;
      } else if (opString == "R14") {
        return Register::R14;
      } else if (opString == "R15") {
        return Register::R15;
      };
  }
}

const Operand getOp(std::string opString) {
  OpType t = getOpType(opString);
  int v = getOpVal(t, opString);
  Operand op = {t, v};
  if (t == Reg) {
    Mode m = getMode(opString);
    op.mode = m;
  }
  return op;
}

const Mnemonic getMnemonic(std::string instString) {
  if (instString == "MOV") {
    return MOV;
  } else if (instString == "ADD") {
    return ADD;
  } else {
    throw;
  }
}

}  // namespace Hex

#endif  // HEXHELPER_HPP