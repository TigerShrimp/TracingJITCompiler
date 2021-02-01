#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "HexHelper.hpp"

class Assembler {
 private:
 public:
  std::vector<hexCode> assemble(std::vector<std::string>&);
  std::vector<hexCode> assemble(std::vector<Instruction>&);
};

#endif  // ASSEMBLER_HPP