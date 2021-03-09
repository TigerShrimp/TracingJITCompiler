#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <iomanip>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "Program.hpp"
#include "TraceHandler.hpp"

// TigerShrimp interpreter:
// Interpreter for JVM byte-code.
// Interprets JVM byte-code.
// Reads code and does stuff with it.
// Magic.
class Interpreter {
 public:
  void evalInstruction(Program*, Mnemonic, std::vector<Value>);
  std::vector<Value> prepareParams(Program*, Mnemonic);

 private:
  TraceHandler traceHandler;

  size_t findIndexOfMain(Program*);
  void invoke(Program*, size_t);
  int findNameIndex(Program*, size_t);
  // Helper functions
  std::string toString(Value);
  int readParametersAsInt(Program*);
  size_t sizeOf(std::vector<Type>);
  size_t sizeOf(Type);
};

#endif  // INTERPRETER_HPP