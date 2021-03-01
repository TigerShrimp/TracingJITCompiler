#ifndef RUNTIME_HPP
#define RUNTIME_HPP
#include <map>
#include <stack>
#include <vector>

#include "Definitions.hpp"
#include "Interpreter.hpp"
#include "MemoryHandler.hpp"
#include "Program.hpp"
#include "TraceHandler.hpp"

class RunTime {
 public:
  RunTime();
  RunTime(std::vector<uint8_t>, int, int, int);
  void run(Program*);

 private:
  Interpreter interpreter;
  MemoryHandler memoryHandler;
  TraceHandler traceHandler;

  size_t findIndexOfMain(Program*);
  void initProgramState(Program*);
};

#endif  // RUNTIME_HPP