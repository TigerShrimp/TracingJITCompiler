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
#include "JVM/Decoder.hpp"

struct Value {
  Type type;
  union {
    int intValue;
    long longValue;
    float floatValue;
    double doubleValue;
  } val;
  Value();
  Value(int);
  Value(long);
  Value(float);
  Value(double);
};

Value operator+(const Value&, const Value&);
Value operator-(const Value&, const Value&);
Value operator*(const Value&, const Value&);
Value operator/(const Value&, const Value&);
bool operator<(const Value&, const Value&);

struct State {
  size_t pc;
  size_t method;
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

  size_t findIndexOfMain();
  void invoke(size_t);
  size_t findNameIndex(size_t);
  void eval();
  void load(size_t);
  void store(size_t);
  // Helper functions
  Value pop();
  void push(Value);
  int getParametersAsInt();
  size_t sizeOf(std::vector<Type>);
  size_t sizeOf(Type);
};

#endif  // INTERPRETER_HPP