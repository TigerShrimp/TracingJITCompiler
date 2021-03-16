#include "TraceHandler.hpp"

#include "Definitions.hpp"

bool TraceHandler::hasTrace(ProgramCounter pc) {
  bool has = traces.contains(pc);
  DEBUG_PRINT("Trace at ({},{}) = {}\n", pc.methodIndex, pc.instructionIndex,
              has);
  return has;
}

void TraceHandler::runTrace(State* state) {
  DEBUG_PRINT("Will run trace\n");
  void** args = (void**)malloc(sizeof(void*) * state->locals.size());
  DEBUG_PRINT("Writing store\n");
  for (const auto& [key, local] : state->locals) {
    DEBUG_PRINT("Var {} val {} addr {}\n", key, local.val.intValue,
                (void*)&(local.val));
    args[key] = (void*)&(local.val);
  }
  Trace trace = traces[state->pc];
  DEBUG_PRINT("Will run trace at {}\n", (void*)&trace.tracePointer.startAddr);
  size_t exitPoint = trace.tracePointer.execute(args);
  DEBUG_PRINT("Exit point {} = ({},{})\n", exitPoint,
              trace.exitPoints[exitPoint].methodIndex,
              trace.exitPoints[exitPoint].instructionIndex);
  free(args);
  state->pc = trace.exitPoints[exitPoint];
}

void TraceHandler::insertTrace(tracePointer ptr, ProgramCounter startPc,
                               std::map<size_t, ProgramCounter> exitPoints) {
  traces[startPc] = {ptr, exitPoints};
}
void TraceHandler::insertTrace(tracePointer ptr, size_t method_index,
                               size_t start, size_t end) {
  Trace trace = {ptr};
  trace.exitPoints[0] = {end, method_index};
  DEBUG_PRINT("Write trace at: ({},{})", method_index, start)
  traces[{start, method_index}] = trace;
}