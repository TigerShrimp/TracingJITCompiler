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
  Interpreter(TraceHandler, Program);
  void interpret();

 private:
  TraceHandler traceHandler;
  Program program;
  std::stack<State*> states;

  size_t findIndexOfMain();
  void invoke(size_t);
  size_t findNameIndex(size_t);
  void eval();
  void load(size_t);
  void store(size_t);
  // Helper functions
  Value pop();
  void push(Value);
  std::string toString(Value);
  int getParametersAsInt();
  size_t sizeOf(std::vector<Type>);
  size_t sizeOf(Type);
};

#endif  // INTERPRETER_HPP