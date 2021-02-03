#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP
#include <asmjit/x86.h>

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "AsmHelper.hpp"

static const std::regex regReg(
    "RDI|RSI|RDX|RCX|R8|R9|R10|R11|RAX|RBX|R12|R13|R14|R15");
static const std::regex lowRegReg("RDI|RSI|RDX|RCX|RAX|RBX|RSP|RBP");
static const std::regex memReg("\\[RBP(\\+|\\-)[0-9]+\\]");
static const std::regex immReg("^-?[0-9]+$");

enum ArgumentType { Reg, Mem, Imm };

struct Argument {
  ArgumentType type;
  std::string val;
};

class Assembler {
 private:
 public:
  std::vector<uint8_t> assemble(std::vector<std::string>&);
};

static const ArgumentType getArgumentType(std::string argument);
static const asmjit::x86::Gp getReg(std::string regString);
static const Argument getArgument(std::string argString);
static const asmjit::Operand convert(Argument arg);

#endif  // ASSEMBLER_HPP