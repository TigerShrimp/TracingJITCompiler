#include "Program.hpp"

using namespace std;

string Program::programString() {
  stringstream ss;
  for (auto method : methods) {
    ss << ((ConstantUtf8 *)constantPool[method.first])->bytes << "("
       << method.first << "):" << endl;

    ss << method.second.methodString();
  }
  return ss.str();
}

string Method::methodString() {
  stringstream ss;
  for (int i = 0; i < code.size(); i++) {
    Mnemonic mnemonic = static_cast<Mnemonic>(code[i]);
    string s = "";
    int row = i;
    for (int j = 0; j < getParameters(mnemonic); j++) {
      i++;
      s += to_string(code[i]);
      s += " ";
    }
    ss << "  " << row << ": " << byteCodeNames.at(mnemonic) << " " << s << endl;
  }
  return ss.str();
}

uint8_t Program::peek(size_t lookAhead) {
  return methods[states.top()->pc.methodIndex]
      .code[states.top()->pc.instructionIndex + lookAhead];
}

uint8_t Program::readNext() {
  uint8_t nextByte = peek(0);
  states.top()->pc.instructionIndex++;
  return nextByte;
}

Mnemonic Program::readNextMnemonic() {
  return static_cast<Mnemonic>(readNext());
}

void Program::jump(int diff, int parametersRead) {
  states.top()->pc.instructionIndex += diff - parametersRead;
}
void Program::jump(int diff) { jump(diff, 0); }

void Program::push(Value val) { states.top()->stack.push(val); }

Value Program::pop() {
  Value val = states.top()->stack.top();
  states.top()->stack.pop();
  return val;
}

void Program::store(int var) {
  Value val = pop();
  states.top()->locals[var] = val;
}

void Program::load(int var) {
  Value val = states.top()->locals[var];
  push(val);
}

bool operator<(const ProgramCounter &lhs, const ProgramCounter &rhs) {
  return (lhs.methodIndex == rhs.methodIndex &&
          lhs.instructionIndex < rhs.instructionIndex);
}

Value::Value() : Value(0){};

Value::Value(int value) {
  type = {Int};
  val = {.intValue = value};
}

Value::Value(long value) {
  type = {Long};
  val = {.longValue = value};
}

Value::Value(float value) {
  type = {Float};
  val = {.floatValue = value};
}

Value::Value(double value) {
  type = {Double};
  val = {.doubleValue = value};
}

Value operator+(const Value &lhs, const Value &rhs) {
  switch (lhs.type.type) {
    case Int:
      return Value(lhs.val.intValue + rhs.val.intValue);
    case Long:
      return Value(lhs.val.longValue + rhs.val.longValue);
    case Float:
      return Value(lhs.val.floatValue + rhs.val.floatValue);
    case Double:
      return Value(lhs.val.doubleValue + rhs.val.doubleValue);
    default:
      return NULL;
  }
}
Value operator-(const Value &lhs, const Value &rhs) {
  switch (lhs.type.type) {
    case Int:
      return Value(lhs.val.intValue - rhs.val.intValue);
    case Long:
      return Value(lhs.val.longValue - rhs.val.longValue);
    case Float:
      return Value(lhs.val.floatValue - rhs.val.floatValue);
    case Double:
      return Value(lhs.val.doubleValue - rhs.val.doubleValue);
    default:
      return NULL;
  }
}
Value operator*(const Value &lhs, const Value &rhs) {
  switch (lhs.type.type) {
    case Int:
      return Value(lhs.val.intValue * rhs.val.intValue);
    case Long:
      return Value(lhs.val.longValue * rhs.val.longValue);
    case Float:
      return Value(lhs.val.floatValue * rhs.val.floatValue);
    case Double:
      return Value(lhs.val.doubleValue * rhs.val.doubleValue);
    default:
      return NULL;
  }
}
Value operator/(const Value &lhs, const Value &rhs) {
  switch (lhs.type.type) {
    case Int:
      return Value(lhs.val.intValue / rhs.val.intValue);
    case Long:
      return Value(lhs.val.longValue / rhs.val.longValue);
    case Float:
      return Value(lhs.val.floatValue / rhs.val.floatValue);
    case Double:
      return Value(lhs.val.doubleValue / rhs.val.doubleValue);
    default:
      return NULL;
  }
}
bool operator<(const Value &lhs, const Value &rhs) {
  switch (lhs.type.type) {
    case Int:
      return lhs.val.intValue < rhs.val.intValue;
    case Long:
      return lhs.val.longValue < rhs.val.longValue;
    case Float:
      return lhs.val.floatValue < rhs.val.floatValue;
    case Double:
      return lhs.val.doubleValue < rhs.val.doubleValue;
    default:
      return false;
  }
}