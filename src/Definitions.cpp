#include "Definitions.hpp"

void debugPrint(std::stringstream &stream) {
#ifdef DEBUG_PRINT
  std::cout << stream.str();
#endif
  stream.clear();
}