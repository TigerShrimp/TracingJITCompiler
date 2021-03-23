#include "x86.hpp"

bool operator<(const Op &lhs, const Op &rhs) {
  if (lhs.opType != rhs.opType)
    return lhs.opType < rhs.opType;
  else
    switch (lhs.opType) {
      case REGISTER:
        return lhs.reg < rhs.reg;
      case XMM_REGISTER:
        return lhs.xreg < rhs.xreg;
      case MEMORY:
        return lhs.mem.reg == rhs.mem.reg ? lhs.mem.offset < rhs.mem.offset
                                          : lhs.mem.reg < rhs.mem.reg;
      case IMMEDIATE:
        return lhs.val < rhs.val;
      case LABEL:
        return lhs.pc < rhs.pc;
    }
}

bool operator==(const Op &lhs, const Op &rhs) {
  return !(lhs < rhs || rhs < lhs);
}