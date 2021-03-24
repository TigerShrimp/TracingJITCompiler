#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include "Assembler.hpp"
#include "Definitions.hpp"
#include "JVM/ClassFile.hpp"
#include "JVM/Decoder.hpp"
#include "JVM/Parser.hpp"
#include "MemoryHandler.hpp"
#include "RunTime.hpp"
#include "TraceHandler.hpp"

using namespace std;

void readFile(string, vector<string>&);

void printUsage() {
  cout << "Usage: TigerShrimp (file.class) | (file.class file.asm "
          "method_name_index (int) start_trace (int) end_trace (int))"
       << endl;
}

void printError(string error, bool showUsage) {
  cerr << "TigerShrimp:\033[1;31m error: \033[0m" << error << endl;
  if (showUsage) printUsage();
}

void interpretJava(string path, RunTime runTime) {
  Parser parser;
  Decoder decoder;
  ClassFile cf = parser.parse(path);
  Program* prg = decoder.decode(cf);
  DEBUG_PRINT("{}", cf.contentsString());
  DEBUG_PRINT("{}", prg->programString());

  runTime.run(prg);
  DEBUG_PRINT("Program exited\n");
  delete prg;
}

RunTime assembleAssembly(string path, int methodIndex, int start, int end) {
  vector<string> asmRows;
  readFile(path, asmRows);
  for (auto row : asmRows) {
    DEBUG_PRINT("{}\n", row);
  }
  Assembler assembler;
  vector<uint8_t> traceBytes = assembler.assemble(asmRows);
  return RunTime(traceBytes, methodIndex, start, end);
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
        interpretJava(path, {});
      } else if (extension == "asm") {
        printError("Supportn't assemble assembly any more", true);
      }
      break;
    }
    case 6: {
      string javaPath(args[1]);
      string asmPath(args[2]);
      int methodIndex = stoi(args[3]);
      int start = stoi(args[4]);
      int end = stoi(args[5]);
      RunTime rt = assembleAssembly(asmPath, methodIndex, start, end);
      interpretJava(javaPath, rt);
      break;
    }
    default:
      printError("Numbern't of arguments", true);
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