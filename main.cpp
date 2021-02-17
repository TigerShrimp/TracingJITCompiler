#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include "Assembler.hpp"
#include "Definitions.hpp"
#include "Interpreter.hpp"
#include "JVM/ClassFile.hpp"
#include "JVM/Decoder.hpp"
#include "JVM/Parser.hpp"
#include "MemoryHandler.hpp"

using namespace std;

void readFile(string, vector<string>&);

void printUsage() { cout << "Usage: TigerShrimp file.asm|.class" << endl; }

void printError(string error, bool showUsage) {
  cerr << "TigerShrimp:\033[1;31m error: \033[0m" << error << endl;
  if (showUsage) printUsage();
}

void interpretJava(string path) {
  Parser parser;
  Decoder decoder;
  ClassFile cf = parser.parse(path);
  Program prg = decoder.decode(cf);
#ifdef DEBUG_PRINT
  prg.debugPrint();
#endif
  Interpreter interpreter(prg);
  interpreter.interpret();
}

void assembleAssembly(string path) {
  vector<string> asmRows;
  readFile(path, asmRows);
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

int main(int argc, char** args) {
  switch (argc) {
    case 1:
      printUsage();
      exit(EXIT_FAILURE);
      break;
    case 2: {
      string path(args[1]);
      string extension = path.substr(path.find_last_of(".") + 1);
      if (extension == "class") {
        interpretJava(path);
      } else if (extension == "asm") {
        assembleAssembly(path);
      }
      break;
    }
    default:
      printError("too many arguments", true);
      exit(EXIT_FAILURE);
      break;
  }
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