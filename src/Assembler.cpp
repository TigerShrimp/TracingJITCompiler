#include "Assembler.hpp"

using namespace std;
// Private functions

// Public functions
vector<hexCode> Assembler::assemble(vector<string> &asmRows) {
  vector<Instruction> instructionStream;
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
    Instruction instruction = {Hex::getMnemonic(instParts[0]), {None}, {None}};
    switch (instParts.size()) {
      case 3:
        instruction.op2 = Hex::getOp(instParts[2]);
      case 2:
        instruction.op1 = Hex::getOp(instParts[1]);
        break;
    }
    instructionStream.push_back(instruction);
  }
  return this->assemble(instructionStream);
}

vector<hexCode> Assembler::assemble(vector<Instruction> &instructionStream) {
  vector<hexCode> hexStream;
  for (auto instruction : instructionStream) {
    hexCode mode = Hex::getMode(instruction);
    if (mode != 0) {
      hexStream.push_back(mode);
    }
  }
  return hexStream;
}