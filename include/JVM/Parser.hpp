#ifndef JVM_PARSER_HPP
#define JVM_PARSER_HPP
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ClassFile.hpp"

class Parser {
 public:
  ClassFile parse(std::string);

 private:
  ClassFile parse(std::vector<uint8_t>);
  std::map<size_t, CPInfo*> parseConstantPoolInfo(
      std::vector<uint8_t>::iterator&);
  std::vector<uint16_t> parseInterfaces(std::vector<uint8_t>::iterator&);
  std::vector<FieldInfo> parseFields(std::vector<uint8_t>::iterator&,
                                     std::map<size_t, CPInfo*>);
  std::vector<MethodInfo> parseMethods(std::vector<uint8_t>::iterator&,
                                       std::map<size_t, CPInfo*>);
  std::map<std::string, AttributeInfo*> parseAttributeInfo(
      std::vector<uint8_t>::iterator&, std::map<size_t, CPInfo*>&);
  std::vector<VerificationInfo> parseVerificationInfo(
      std::vector<uint8_t>::iterator&, uint16_t);

  uint8_t readU1(std::vector<uint8_t>::iterator&);
  uint16_t readU2(std::vector<uint8_t>::iterator&);
  uint32_t readU4(std::vector<uint8_t>::iterator&);
};

#endif  // JVM_PARSER_HPP