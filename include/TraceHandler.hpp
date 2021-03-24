#ifndef TRACEHANDLER_HPP
#define TRACEHANDLER_HPP
#include <map>

#include "MemoryHandler.hpp"
#include "Program.hpp"

struct Trace {
  TracePointer tracePointer;
  std::map<long, ProgramCounter> exitPoints;
  size_t maxLocals;
};

class TraceHandler {
 public:
  bool hasTrace(ProgramCounter);
  void runTrace(State*);
  void insertTrace(ProgramCounter, Trace);
  void insertTrace(TracePointer, ProgramCounter,
                   std::map<size_t, ProgramCounter>);
  void insertTrace(TracePointer, size_t, size_t, size_t);

 private:
  std::map<ProgramCounter, Trace> traces;
};

#endif  // TRACEHANDLER_HPP