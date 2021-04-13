#include "TraceHandler.hpp"

#include "Definitions.hpp"

bool TraceHandler::hasTrace(ProgramCounter pc) {
  bool has = traces.contains(pc);
  DEBUG_PRINT("Trace at ({},{}) = {}\n", pc.methodIndex, pc.instructionIndex,
              has);
  return has;
}

// Returns the pc at which the interpreter should take back control
// Since only side exits and not the "default" exit will be hot we can
// count each time we exit.
ProgramCounter TraceHandler::runTrace(State* state) {
  DEBUG_PRINT("Will run trace\n");
  Trace trace = traces[state->pc];
  void** args = (void**)malloc(sizeof(void*) * trace.maxLocals);
  DEBUG_PRINT("Writing store, {}\n", state->locals.size());
  for (const auto& [key, local] : state->locals) {
    DEBUG_PRINT("Var {} val {} addr {}\n", key, local.val.intValue,
                (void*)&(local.val));
    args[key] = (void*)&(local.val);
  }
  DEBUG_PRINT("Will run trace at {}\n", (void*)&trace.tracePointer.startAddr);
  size_t exitPoint = trace.tracePointer.execute(args);
  DEBUG_PRINT("Exit point {} = ({},{})\n", exitPoint,
              trace.exitPoints[exitPoint].methodIndex,
              trace.exitPoints[exitPoint].instructionIndex);
  free(args);
  return trace.exitPoints[exitPoint];
}

void TraceHandler::insertTrace(ProgramCounter pc, Trace trace) {
  traces[pc] = trace;
}

void TraceHandler::insertTrace(TracePointer ptr, size_t method_index,
                               size_t start, size_t end) {
  Trace trace = {ptr};
  trace.exitPoints[0] = {end, method_index};
  DEBUG_PRINT("Write trace at: ({},{})", method_index, start)
  traces[{start, method_index}] = trace;
}