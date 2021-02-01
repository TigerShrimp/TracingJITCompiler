#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include "Assembler.hpp"

using namespace std;

void readFile(string, vector<string>&);

int main() {
  vector<string> asmRows;
  readFile("asm.asm", asmRows);
  for (auto row : asmRows) {
    cout << row << endl;
  }
  Assembler assembler;
  assembler.assemble(asmRows);
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