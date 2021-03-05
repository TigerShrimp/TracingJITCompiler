#include "RunTime.hpp"

using namespace std;

RunTime::RunTime() {}
RunTime::RunTime(vector<uint8_t> initialTrace, int method, int start, int end)
    : RunTime() {
  tracePointer tp = memoryHandler.writeTrace(initialTrace);
  traceHandler.insertTrace(tp, method, start, end);
}

bool recordingTrace() { return false; }

void RunTime::run(Program *program) {
  initProgramState(program);
  // Returning from main means the program stack will be empty and the program
  // will terminate.
  while (!program->states.empty()) {
    DEBUG_PRINT("States: {}\n", program->states.size());
    State *state = program->states.top();
    profiler.note(state->pc);
    if (recordingTrace()) {
      cout << "not implemented yet" << endl;
    } else if (traceHandler.hasTrace(state->pc)) {
      traceHandler.runTrace(state);
    } else if (false &&
               profiler.isHot(state->pc)) {  // False for now to not prohibit
                                             // interpreter evaluation
      DEBUG_PRINT("Hot loop found ({},{})\n", state->pc.methodIndex,
                  state->pc.instructionIndex);
      break;
    } else
      interpreter.eval(program);
  }
  memoryHandler.freeTraces();
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
  // TODO: Standardize execption handing.
  cerr << "Class file containsn't a main function" << endl;
  throw;
}