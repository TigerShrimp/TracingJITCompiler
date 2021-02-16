#ifndef DECODER_HPP
#define DECODER_HPP

#include <map>
#include <regex>
#include <string>
#include <vector>

#include "JVM/ByteCodes.hpp"
#include "JVM/ClassFile.hpp"

static const std::regex typesRegex("\\(([^\\)]*)\\)([^$]+)");

enum BaseType { Int, Double, Void, Long, String, List, Float };

struct Type {
  BaseType type;
  Type* subType;
};

struct ByteCode {
  Mnemonic mnemonic;
  std::vector<uint8_t> params;
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
  void debugPrint();
};

struct Program {
  std::vector<CPInfo*> constantPool;
  std::map<uint16_t, Method> methods;
  void debugPrint();
};

class Decoder {
 public:
  Program decode(ClassFile);

 private:
  void parseTypes(CPInfo*, Method&);
  size_t getTypeStringLength(Type);
  Type getTypeFromString(std::string);
};

#endif  // DECODER_HPP