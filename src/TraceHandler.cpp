#include "TraceHandler.hpp"

#include "Definitions.hpp"

inline int handleTraceExit(ExitInformation* info, int exitId) {
  ProgramCounter exitPc = info->exitPoints->at(exitId);
  if (info->traces->contains(exitPc)) {
    // std::cout << "Contains:" << exitId << std::endl;
    Trace trace = info->traces->at(exitPc);
    // std::cout << "Has:" << exitId << std::endl;
    asm volatile(
        "mov %0, %%rdi;\n\t"
        "mov %1, %%rsi;\n\t"
        "leave;\n\t"
        "jmp *%2;\n\t"
        :
        : "r"(info), "r"((void*)&handleTraceExit),
          "r"((void*)trace.tracePointer.startAddr)
        : "rdi", "rsi");
    // return trace.tracePointer.execute((void*)info, (void*)&handleTraceExit);
  }
  return exitId;
}

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

  ExitInformation* info = new ExitInformation{args, &exitPoints, &traces};
  DEBUG_PRINT("Will run trace at 0x{:x}\n",
              (size_t)trace.tracePointer.startAddr);
  int exitPoint =
      trace.tracePointer.execute((void*)info, (void*)&handleTraceExit);
  DEBUG_PRINT("Exit point {} = ({},{})\n", exitPoint,
              exitPoints[exitPoint].methodIndex,
              exitPoints[exitPoint].instructionIndex);
  free(args);
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