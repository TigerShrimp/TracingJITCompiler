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

#ifdef INTERPRETING_ONLY
void RunTime::run(Program *program) {
  initProgramState(program);
#ifdef TIMING
  auto before = chrono::time_point_cast<chrono::microseconds>(
      chrono::system_clock::now());
#endif
  while (!program->states.empty()) {
    ByteCodeInstruction inst = interpreter.prepareNext(program);
    interpreter.evalInstruction(program, inst);
  }
#ifdef TIMING
  auto now = chrono::system_clock::now();
  size_t elapsed = (now - before).count();
  std::cout << elapsed << std::endl;
#endif
}
#else
void RunTime::run(Program *program) {
  // BEFORE_RUN
  initProgramState(program);
#ifdef TIMING
  // Start timing after Java program has been parsed.
  auto before = chrono::time_point_cast<chrono::microseconds>(
      chrono::system_clock::now());
#endif
  // Returning from main means the program stack will be
  // empty and the program will terminate.
  while (!program->states.empty()) {
    DEBUG_PRINT("States: {}, recording: {}\n", program->states.size(),
                traceRecorder.isRecording());
    State *state = program->states.top();
    ProgramCounter pc = state->pc;
    if (traceRecorder.isRecording() && traceRecorder.recordingDone(pc)) {
      DEBUG_PRINT("Recording done -- COMPILING\n")
      Recording recording = traceRecorder.getRecording();
      Trace trace = compiler.compileAndInstall(
          program->methods[pc.methodIndex].maxLocals, recording);
      // COMPILING_DONE
      traceHandler.insertTrace(recording.startPc, trace);
    }
    if (traceHandler.hasTrace(pc)) {
      // NATIVE_TRACE
      ProgramCounter exitPc = traceHandler.runTrace(state);
      profiler.countSideExitFor(exitPc);
      if (profiler.isHot(exitPc)) {
        // pc is the program counter before entering the trace, i.e. the
        // header of the loop where the trace starts.
        ProgramCounter loopHeaderPc = pc;
        traceRecorder.initRecording(loopHeaderPc, exitPc);
        DEBUG_PRINT("\tHot side exit found ({},{})\n", exitPc.methodIndex,
                    exitPc.instructionIndex);
      }
      state->pc = exitPc;
    } else {
      ByteCodeInstruction inst = interpreter.prepareNext(program);
      profiler.countVisitFor(pc);
      if (profiler.isHot(pc)) {
        DEBUG_PRINT("\tHot loop found ({},{})\n", pc.methodIndex,
                    pc.instructionIndex);
        // INIT_RECORDING
        traceRecorder.initRecording(pc);
      }
      if (traceRecorder.isRecording()) {
        traceRecorder.record(pc, inst);
      }
      // INTERPRETING
      interpreter.evalInstruction(program, inst);
    }
  }
#ifdef TIMING
  auto now = chrono::system_clock::now();
  size_t elapsed = (now - before).count();
  std::cout << elapsed << std::endl;
#endif
#ifdef TRACE_INFO
  int traces = traceHandler.traceCount();
  size_t bytesWritten = compiler.bytesWritten();
  std::cout << traces << "," << bytesWritten << std::endl;
#endif
}
#endif

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
