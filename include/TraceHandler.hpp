#ifndef TRACEHANDLER_HPP
#define TRACEHANDLER_HPP
#include <map>

#include "MemoryHandler.hpp"
#include "Program.hpp"

struct Trace {
  TracePointer tracePointer;
  std::map<size_t, ProgramCounter> exitPoints;
};

class TraceHandler {
 public:
  bool hasTrace(ProgramCounter);
  void runTrace(State*);
  void insertTrace(TracePointer, ProgramCounter,
                   std::map<size_t, ProgramCounter>);
  void insertTrace(TracePointer, size_t, size_t, size_t);

 private:
  std::map<ProgramCounter, Trace> traces;
};

#endif  // TRACEHANDLER_HPP