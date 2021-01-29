#ifndef HEXHELPER_HPP
#define HEXHELPER_HPP
#include <string>
#include <vector>

enum OpType { Reg, Mem, Imm };

struct Op {
  OpType type;
  std::string val;
};

typedef unsigned char hexCode;

namespace Hex {

#define OP_SIZE_64 0x48

#define MOV "MOV"
#define MOV_RM_I 0xC7
#define MOV_RM_R 0x88
#define MOV_R_RM 0x89

}  // namespace Hex

#endif  // HEXHELPER_HPP