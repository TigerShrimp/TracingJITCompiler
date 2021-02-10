#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "JVM/ClassFile.hpp"

// TigerShrimp interpreter:
// Interpreter for JVM byte-code.
// Interprets JVM byte-code.
// Reads code and does stuff with it.
// Magic.
class Interpreter {
 public:
  Interpreter();
  void interpret(ClassFile);

 private:
  size_t pc;
  std::map<std::string, std::vector<uint8_t>> methods;
};

#endif  // INTERPRETER_HPP