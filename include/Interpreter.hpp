#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "JVM/ByteCodes.hpp"
#include "JVM/Decoder.hpp"

union Value {
  int intValue;
  double doubleValue;
  long longValue;
  float floatValue;
};

struct State {
  size_t pc;
  uint16_t method;
  std::stack<Value> stack;
  std::map<size_t, Value> locals;
};

// TigerShrimp interpreter:
// Interpreter for JVM byte-code.
// Interprets JVM byte-code.
// Reads code and does stuff with it.
// Magic.
class Interpreter {
 public:
  Interpreter(Program);
  void interpret();

 private:
  Program program;
  size_t pc;
  std::stack<State*> states;

  uint16_t findIndexOfMain();
  void invoke(uint16_t);
  void eval();
  void load(size_t);
  void store(size_t);
  // Helper functions
  Value pop();
  void push(Value);
  int getParametersAsInt();
  size_t sizeOf(Type);
};

#endif  // INTERPRETER_HPP