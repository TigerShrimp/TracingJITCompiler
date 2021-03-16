#ifndef RUNTIME_HPP
#define RUNTIME_HPP
#include <map>
#include <stack>
#include <vector>

#include "ByteCodes.hpp"
#include "Compiler.hpp"
#include "Definitions.hpp"
#include "Interpreter.hpp"
#include "MemoryHandler.hpp"
#include "Profiler.hpp"
#include "Program.hpp"
#include "TraceHandler.hpp"
#include "TraceRecorder.hpp"

class RunTime {
 public:
  RunTime();
  RunTime(std::vector<uint8_t>, int, int, int);
  void run(Program*);

 private:
  Compiler compiler;
  Interpreter interpreter;
  Profiler profiler;
  MemoryHandler memoryHandler;
  TraceHandler traceHandler;
  TraceRecorder traceRecorder;

  size_t findIndexOfMain(Program*);
  void initProgramState(Program*);
};

#endif  // RUNTIME_HPP