#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP
#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "HexHelper.hpp"

static const std::regex regReg(
    "RDI|RSI|RDX|RCX|R8|R9|R10|R11|RAX|RBX|R12|R13|R14|R15");
static const std::regex memReg("\\[RBP\\+[0-9]+\\]");
static const std::regex immReg("^0x[0-9A-F]+$|^0b[0-1]+$|^[0-9]+$");

class Assembler {
 private:
  OpType getOpType(std::string);

 public:
  std::vector<hexCode> assemble(std::vector<std::string>&);
};

#endif  // ASSEMBLER_HPP