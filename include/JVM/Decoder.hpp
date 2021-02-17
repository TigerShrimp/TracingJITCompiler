#ifndef DECODER_HPP
#define DECODER_HPP

#include <map>
#include <regex>
#include <string>
#include <vector>

#include "Definitions.hpp"
#include "JVM/ByteCodes.hpp"
#include "JVM/ClassFile.hpp"
#include "Program.hpp"

static const std::regex typesRegex("\\(([^\\)]*)\\)([^$]+)");

class Decoder {
 public:
  Program decode(ClassFile);

 private:
  void parseTypes(CPInfo*, Method&);
  size_t getTypeStringLength(Type);
  Type getTypeFromString(std::string);
};

#endif  // DECODER_HPP