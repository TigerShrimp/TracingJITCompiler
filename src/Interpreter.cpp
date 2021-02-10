#include "Interpreter.hpp"

Interpreter::Interpreter() : pc(0) {
  methods["main"] = {
      0x01,
      0x02,
      0x03,
      0x04,
  };
};

void Interpreter::interpret(ClassFile classFile) {
  // Parse classFile  methods to instructions
  int i = 0;
  while (++pc < 100) {
    i += pc;
    uint8_t inst = methods["main"][pc % 4];
    i -= inst;
  }
}