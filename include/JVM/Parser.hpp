#ifndef JVM_PARSER_HPP
#define JVM_PARSER_HPP
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "JVM/ClassFile.hpp"

class Parser {
 public:
  ClassFile parse(std::string);

 private:
  ClassFile parse(std::vector<uint8_t>);

  uint8_t readU1(std::vector<uint8_t>::iterator&);
  uint16_t readU2(std::vector<uint8_t>::iterator&);
  uint32_t readU4(std::vector<uint8_t>::iterator&);
};

#endif  // JVM_PARSER_HPP