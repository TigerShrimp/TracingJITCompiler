#ifndef PROGRAM_HPP
#define PROGRAM_HPP
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>

#include "JVM/ByteCodes.hpp"
#include "JVM/ClassFile.hpp"

enum BaseType { Int, Long, Float, Double, Void, String, List };

struct Type {
  BaseType type;
  Type* subType;
};

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
  std::string toString();
};

Value operator+(const Value&, const Value&);
Value operator-(const Value&, const Value&);
Value operator*(const Value&, const Value&);
Value operator/(const Value&, const Value&);
bool operator<(const Value&, const Value&);

struct ProgramCounter {
  size_t instructionIndex;
  size_t methodIndex;
};

bool operator<(const ProgramCounter&, const ProgramCounter&);
bool operator==(const ProgramCounter&, const ProgramCounter&);

struct ByteCodeInstruction {
  Mnemonic mnemonic;
  std::vector<Value> params;
};

struct State {
  ProgramCounter pc;
  std::stack<Value> stack;
  std::map<size_t, Value> locals;
};

struct Method {
 public:
  uint16_t nameIndex;
  Type retType;
  std::vector<Type> argTypes;
  uint16_t maxStack;
  uint16_t maxLocals;
  std::vector<uint8_t> code;
  ConstantValueAttribute constant;
  StackMapTableAttribute stackMapTable;
  std::string methodString();
};

struct Program {
  std::map<size_t, CPInfo*> constantPool;
  std::map<uint16_t, Method> methods;
  std::stack<State*> states;
  uint8_t peek(size_t);
  uint8_t readNext();
  Mnemonic readNextMnemonic();
  void jump(int);
  void jump(int, int);
  void push(Value);
  Value pop();
  void store(int);
  void load(int);
  std::string programString();
};

#endif  // PROGRAM_HPP