#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include "Assembler.hpp"
#include "MemoryHandler.hpp"

using namespace std;

void readFile(string, vector<string>&);

int main() {
  vector<string> asmRows;
  readFile("../asm.asm", asmRows);
  for (auto row : asmRows) {
    cout << row << endl;
  }
  Assembler assembler;
  vector<uint8_t> traceBytes = assembler.assemble(asmRows);
  MemoryHandler memoryHandler;
  tracePointer tp = memoryHandler.writeTrace(traceBytes);
  size_t res = tp.execute();
  cout << "Res " << res << endl;
}

void readFile(string path, vector<string>& rows) {
  ifstream in(path);
  string str;
  while (getline(in, str)) {
    if (str.size() > 0) {
      rows.push_back(str);
    }
  }
  in.close();
}