#ifndef RUNTIME_HPP
#define RUNTIME_HPP
#include <map>
#include <stack>
#include <vector>

#include "ByteCodes.hpp"
#include "Definitions.hpp"
#include "Interpreter.hpp"
#include "MemoryHandler.hpp"
#include "Profiler.hpp"
#include "Program.hpp"
#include "TraceHandler.hpp"

class RunTime {
 public:
  RunTime();
  RunTime(std::vector<uint8_t>, int, int, int);
  void run(Program*);

 private:
  Interpreter interpreter;
  Profiler profiler;
  MemoryHandler memoryHandler;
  TraceHandler traceHandler;

  size_t findIndexOfMain(Program*);
  void initProgramState(Program*);
};

#endif  // RUNTIME_HPP