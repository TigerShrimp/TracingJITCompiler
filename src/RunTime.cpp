#include "RunTime.hpp"

using namespace std;

RunTime::RunTime() {}
RunTime::RunTime(vector<uint8_t> initialTrace, int method, int start, int end)
    : RunTime() {
  // TODO: Either move this functionality to Compiler or remove
  // TracePointer tp = memoryHandler.writeTrace(initialTrace);
  // traceHandler.insertTrace(tp, method, start, end);
}

bool recordingTrace() { return false; }

void RunTime::run(Program *program) {
  initProgramState(program);
  // Returning from main means the program stack will be
  // empty and the program will terminate.
  while (!program->states.empty()) {
    DEBUG_PRINT("States: {}\n", program->states.size());
    State *state = program->states.top();
    if (traceHandler.hasTrace(state->pc)) {
      traceHandler.runTrace(state);
    } else {
      ProgramCounter pc = state->pc;
      ByteCodeInstruction inst = interpreter.prepareNext(program);
      profiler.countVisitFor(pc);
      if (!traceRecorder.isRecording() && profiler.isHot(pc)) {
        DEBUG_PRINT("Hot loop found ({},{}) stack size: {}\n",
                    state->pc.methodIndex, state->pc.instructionIndex,
                    state->stack.size());
        traceRecorder.initRecording(pc);
      }
      if (traceRecorder.isRecording()) {
        bool recordingDone = traceRecorder.record(pc, inst);
        if (recordingDone) {
          Recording recording = traceRecorder.getRecording();
          Trace trace = compiler.compileAndInstall(
              program->methods[pc.methodIndex].maxLocals, recording);
          traceHandler.insertTrace(pc, trace);
          state->pc = pc;
          continue;
        }
      }
      interpreter.evalInstruction(program, inst);
    }
  }
}

void RunTime::initProgramState(Program *program) {
  size_t main = findIndexOfMain(program);
  stack<Value> stack;
  map<size_t, Value> locals;
  State *state = new State{0, main, stack, locals};
  program->states.push(state);
}

size_t RunTime::findIndexOfMain(Program *program) {
  for (auto method : program->methods) {
    uint16_t nameIndex = method.first;
    string constantString =
        ((ConstantUtf8 *)program->constantPool[nameIndex])->bytes;
    if (constantString == "main") {
      return nameIndex;
    }
  }
  // TODO: Standardize exception handing.
  cerr << "Class file containsn't a main function" << endl;
  throw;
}

map<string, map<size_t, ByteCodeInstruction>> RunTime::constructProgramString(
    Program *program) {
  map<string, map<size_t, ByteCodeInstruction>> methods;
  for (auto method : program->methods) {
    string key = fmt::format(
        "{} ({})", ((ConstantUtf8 *)program->constantPool[method.first])->bytes,
        method.first);
    map<size_t, ByteCodeInstruction> insts;
    program->states.push(new State{0, method.first});
    while (program->states.top()->pc.instructionIndex <
           method.second.code.size()) {
      size_t pc = program->states.top()->pc.instructionIndex;
      insts[pc] = interpreter.prepareNext(program);
    }
    methods[key] = insts;
    program->states.pop();
  }
  return methods;
}