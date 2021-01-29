#include "Assembler.hpp"

using namespace std;
// Private functions
OpType Assembler::getOpType(string operand) {
  OpType op;
  if (regex_match(operand.begin(), operand.end(), immReg)) {
    op = OpType::Imm;
  } else if (regex_match(operand.begin(), operand.end(), memReg)) {
    op = OpType::Mem;
  } else if (regex_match(operand.begin(), operand.end(), regReg)) {
    op = OpType::Reg;
  } else {
    throw;
  }
  return op;
}

// Public functions
vector<hexCode> Assembler::assemble(vector<string> &asmRows) {
  vector<hexCode> hexCodeStream;
  for (auto asmRow : asmRows) {
    transform(asmRow.begin(), asmRow.end(), asmRow.begin(), ::toupper);
    replace(asmRow.begin(), asmRow.end(), ',', ' ');
    vector<string> instParts;
    string part;
    stringstream ss(asmRow);
    while (getline(ss, part, ' ')) {
      if (part.size() != 0) {
        instParts.push_back(part);
      }
    }
    if (instParts.size() == 3) {
      OpType op1 = getOpType(instParts[1]);
      OpType op2 = getOpType(instParts[2]);
      switch (op2) {
        case ::Imm:
          switch (op1) {
            case ::Mem:
            case ::Reg:
              // om inst == MOV: -> 0xC7

              break;
            default:
              throw;
          }
          break;
        case ::Mem:
          break;
        case ::Reg:
          break;
      }
    } else if (instParts.size() == 2) {
      cout << instParts[0] << ": " << instParts[1] << std::endl;
    } else {
      cout << instParts[0] << std::endl;
    }
  }
  return hexCodeStream;
}