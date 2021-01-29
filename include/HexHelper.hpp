#ifndef HEXHELPER_HPP
#define HEXHELPER_HPP
#include <regex>
#include <string>
#include <vector>

typedef unsigned char hexCode;

enum OpType { Reg, Mem, Imm };

struct Operand {
  OpType type;
  int val;
  hexCode mode;
};

enum Mnemonic {
  MOV,
  ADD
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

const hexCode getMode(std::string opString) {
  return regex_match(opString.begin(), opString.end(), lowRegReg) ? 0x48 : 0x4d;
}

const void addHex(Operand op, std::vector<hexCode>& hexSequence) {
  switch (op.type) {
    case ::Imm:
      // adsadsad;
      break;
    default:
      hexSequence.push_back(calcHex(op));
  }
}

const hexCode combine(hexCode dst, hexCode src) {}

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
      // Japp
      break;
  }
}

const Operand getOp(std::string opString) {
  OpType t = getOpType(opString);
  int v = getOpVal(t, opString);
  Operand op = {t, v};
  if (t == Reg) {
    hexCode m = getMode(opString);
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