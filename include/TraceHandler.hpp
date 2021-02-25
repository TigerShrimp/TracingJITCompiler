#ifndef TRACEHANDLER_HPP
#define TRACEHANDLER_HPP
#include "MemoryHandler.hpp"
#include "Program.hpp"

struct Trace {
  tracePointer tracePointer;
  std::map<size_t, size_t> exitPoints;
};

class TraceHandler {
 public:
  bool hasTrace(State*);
  size_t runTrace(State*);
  void insertTrace(tracePointer, size_t, size_t, size_t);

 private:
  std::map<size_t, std::map<size_t, Trace> > traces;
};

#endif  // TRACEHANDLER_HPP