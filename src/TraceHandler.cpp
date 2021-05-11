#include "TraceHandler.hpp"

#include "Definitions.hpp"

asm("_handleTraceExit:;"
    "push %rdi;"
    "mov $8, %r8;"
    "mov (%rdi, %r8), %rdi;"
    "mov (%rdi, %rsi, 8), %r8;"
    "pop %rdi;"
    "cmp $0, %r8;"
    "je return;"
    "lea _handleTraceExit(%rip), %rsi;"
    "jmp *%r8;"
    "return:;"
    "mov %rsi, %rax;"
    "ret;");
// int handleTraceExit(ExitInformation* info, int exitId) {
//   if (info->traces[exitId] != 0) {
//     TracePointer tracePointer;
//     tracePointer.startAddr = info->traces[exitId];
//     return tracePointer.execute((void*)info, (void*)(&handleTraceExit));
//   } else {
//     return exitId;
//   }
// }

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
  Value::Data* args =
      (Value::Data*)calloc(sizeof(Value::Data), trace.maxLocals);
  DEBUG_PRINT("Writing store, {}\n", state->locals.size());
  for (auto& [key, local] : state->locals) {
    args[key].longValue = (long)state->locals[key].val.intValue;
  }
  uint8_t** tracesArray = (uint8_t**)calloc(sizeof(size_t*), exitPoints.size());
  for (const auto& [id, exitPc] : exitPoints) {
    if (traces.contains(exitPc)) {
      tracesArray[id] = traces[exitPc].tracePointer.startAddr;
    }
  }
  ExitInformation* info = new ExitInformation{args, tracesArray};

  DEBUG_PRINT("Will run trace at 0x{:x}\n",
              (size_t)trace.tracePointer.startAddr);
  int exitPoint =
      trace.tracePointer.execute((void*)info, (void*)&handleTraceExit);
  for (const auto& [key, local] : state->locals) {
    state->locals[key].val.intValue = (int)args[key].longValue;
  }
  free(tracesArray);
  free((void*)args);
  delete info;
  return exitPoints[exitPoint];
}

void TraceHandler::insertTrace(ProgramCounter pc, Trace trace) {
  DEBUG_PRINT("Inserted trace for ({},{}) at address: 0x{:x}\n", pc.methodIndex,
              pc.instructionIndex, (size_t)trace.tracePointer.startAddr);
  traces[pc] = trace;
  for (auto& [id, exitPc] : trace.exitPoints) {
    DEBUG_PRINT("Exit point {} -> ({},{}) added\n", id, exitPc.methodIndex,
                exitPc.instructionIndex);
    assert(!exitPoints.contains(id) && "Cannot have more than one same exitId");
    exitPoints[id] = exitPc;
  }
}

void TraceHandler::insertTrace(TracePointer ptr, size_t method_index,
                               size_t start, size_t end) {
  Trace trace = {ptr};
  trace.exitPoints[0] = {end, method_index};
  DEBUG_PRINT("Write trace at: ({},{})", method_index, start)
  traces[{start, method_index}] = trace;
}

int TraceHandler::traceCount() { return traces.size(); }