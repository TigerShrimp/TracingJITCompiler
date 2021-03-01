#include "TraceHandler.hpp"

#include "Definitions.hpp"

bool TraceHandler::hasTrace(State* state) {
  bool has = traces.contains(state->method) &&
             traces[state->method].contains(state->pc);
  DEBUG_PRINT("{}\n", has);
  return has;
}

size_t TraceHandler::runTrace(State* state) {
  DEBUG_PRINT("Will run trace\n");
  void** args = (void**)malloc(sizeof(void*) * state->locals.size());
  size_t index = 0;
  DEBUG_PRINT("Writing store\n");
  for (const auto& [key, local] : state->locals) {
    DEBUG_PRINT("Var {} val {} addr {}\n", key, local.val.intValue,
                (void*)&(local.val.intValue));

    args[index] = (void*)&(local.val);
    index++;
  }
  Trace trace = traces[state->method][state->pc];
  DEBUG_PRINT("Will run trace at {}\n", (void*)&trace.tracePointer.startAddr);
  size_t exitPoint = trace.tracePointer.execute(args);
  free(args);
  return trace.exitPoints[exitPoint];
}

void TraceHandler::insertTrace(tracePointer ptr, size_t method_index,
                               size_t start, size_t end) {
  Trace trace = {ptr};
  trace.exitPoints[0] = end;
  traces[method_index][start] = trace;
}