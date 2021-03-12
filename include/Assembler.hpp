#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP
#include <asmjit/x86.h>

#include <algorithm>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "AsmHelper.hpp"
#include "Definitions.hpp"
#include "x86.hpp"

static const std::regex regReg(
    "RDI|RSI|RDX|RCX|RAX|RBX|RBP|RSI|R\\d{1,2}|XMM\\d{1,2}");
static const std::regex lowRegReg("RDI|RSI|RDX|RCX|RAX|RBX|RSP|RBP");
static const std::regex immReg("^-?[0-9.]+$");
static const std::regex ptrReg("\\[(R[A-Z0-9]{1,2})((?:\\+|\\-)\\d+)?\\]");

enum ArgumentType { Reg, Mem, Imm };

struct Argument {
  ArgumentType type;
  std::string val;
};

class Assembler {
 private:
 public:
  // Assembles lines of x86_64 instruction strings
  std::vector<uint8_t> assemble(std::vector<std::string>&);
  // Assembles a vector of x86 Instructions defined in x86.hpp
  std::vector<uint8_t> assemble(std::vector<Instruction>&,
                                std::set<ProgramCounter>);
};

static const ArgumentType getArgumentType(std::string argument);
static const asmjit::x86::Gp getReg(std::string regString);
static const Argument getArgument(std::string argString);
static const asmjit::Operand convert(Argument arg);

#endif  // ASSEMBLER_HPP