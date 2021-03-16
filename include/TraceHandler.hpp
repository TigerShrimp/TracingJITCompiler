#ifndef TRACEHANDLER_HPP
#define TRACEHANDLER_HPP
#include <map>

#include "MemoryHandler.hpp"
#include "Program.hpp"

struct Trace {
  tracePointer tracePointer;
  std::map<size_t, ProgramCounter> exitPoints;
};

class TraceHandler {
 public:
  bool hasTrace(ProgramCounter);
  void runTrace(State*);
  void insertTrace(tracePointer, ProgramCounter,
                   std::map<size_t, ProgramCounter>);
  void insertTrace(tracePointer, size_t, size_t, size_t);

 private:
  std::map<ProgramCounter, Trace> traces;
};

#endif  // TRACEHANDLER_HPP