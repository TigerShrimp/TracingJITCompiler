#ifndef RUNTIME_HPP
#define RUNTIME_HPP
#include <fmt/core.h>

#include <map>
#include <stack>
#include <string>
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
  TraceHandler traceHandler;
  TraceRecorder traceRecorder;

  size_t findIndexOfMain(Program*);
  void initProgramState(Program*);
  std::map<std::string, std::map<size_t, ByteCodeInstruction>>
  constructProgramString(Program*);
};

#endif  // RUNTIME_HPP