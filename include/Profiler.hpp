#ifndef PROFILER_HPP
#define PROFILER_HPP
#include <map>

#include "Program.hpp"

class Profiler {
 public:
  bool isHot(ProgramCounter);
  void note(ProgramCounter);

 private:
  static const size_t hotThreshold = 2;
  ProgramCounter lastPc;
  std::map<ProgramCounter, size_t> loopRecord;
};

#endif  // PROFILER_HPP