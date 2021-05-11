#ifndef TRACEHANDLER_HPP
#define TRACEHANDLER_HPP
#include <map>

#include "Program.hpp"

typedef long (*pfunc)(void*, void*);

union TracePointer {
  pfunc execute;
  uint8_t* startAddr;
};

struct Trace {
  TracePointer tracePointer;
  std::map<long, ProgramCounter> exitPoints;
  int maxLocals;
};

struct ExitInformation {
  Value::Data* variables;
  uint8_t** traces;
};

typedef int (*exitfPtr)(ExitInformation*);

extern "C" int handleTraceExit(ExitInformation*, int);

class TraceHandler {
 public:
  bool hasTrace(ProgramCounter);
  ProgramCounter runTrace(State*);
  void insertTrace(ProgramCounter, Trace);
  void insertTrace(TracePointer, ProgramCounter,
                   std::map<size_t, ProgramCounter>);
  void insertTrace(TracePointer, size_t, size_t, size_t);
  int traceCount();

 private:
  std::map<ProgramCounter, Trace> traces;
  std::map<long, ProgramCounter> exitPoints;
};

#endif  // TRACEHANDLER_HPP