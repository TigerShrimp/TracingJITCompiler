#include "Assembler.hpp"

using namespace std;
// Private functions

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
    }
  }
  else if (instParts.size() == 2) {
    cout << instParts[0] << ": " << instParts[1] << std::endl;
  }
  else {
    cout << instParts[0] << std::endl;
  }
}
return hexCodeStream;
}