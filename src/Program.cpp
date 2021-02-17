#include "Program.hpp"

using namespace std;

void Program::debugPrint() {
  for (auto method : methods) {
    cout << ((ConstantUtf8 *)constantPool[method.first])->bytes << ":" << endl;
    method.second.debugPrint();
  }
}

void Method::debugPrint() {
  for (int i = 0; i < code.size(); i++) {
    Mnemonic mnemonic = static_cast<Mnemonic>(code[i]);
    string s = "";
    int row = i;
    for (int j = 0; j < getParameters(mnemonic); j++) {
      i++;
      s += to_string(code[i]);
      s += " ";
    }
    cout << "  " << row << ": " << byteCodeNames.at(mnemonic) << " " << s
         << endl;
  }
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