#include "TraceRecorder.hpp"

using namespace std;
bool TraceRecorder::isRecording() { return recording; }

void TraceRecorder::initRecording(ProgramCounter pc) {
  recording = true;
  traceStart = pc;
  recordedTrace.clear();
  innerBranchTargets.clear();
  outerBranchTargets.clear();
}

Recording TraceRecorder::getRecording() {
  return {recordedTrace, innerBranchTargets, outerBranchTargets};
}

bool TraceRecorder::record(ProgramCounter pc, ByteCodeInstruction inst) {
  if (pc == traceStart && recordedTrace.size() > 0) {
    recording = false;
#ifdef DEBUG_PRINT_ON
    DEBUG_PRINT(" ---- Trace recorded: ({},{}) ----\n", traceStart.methodIndex,
                traceStart.instructionIndex);
    for (auto record : recordedTrace) {
      ByteCodeInstruction bcInst = record.inst;
      DEBUG_PRINT("{} ", JVM::byteCodeNames.at(bcInst.mnemonic));
      for (auto val : bcInst.params) {
        DEBUG_PRINT("{} ", val.toString());
      }
      DEBUG_PRINT("\n")
    }
    DEBUG_PRINT(" ---- ---------------------- ----\n")
#endif
    return true;
  }
  switch (inst.mnemonic) {
    case JVM::GOTO:
    case JVM::IFGT:
    case JVM::IF_ICMPGE:
    case JVM::IF_ICMPGT: {
      ProgramCounter branchTarget = pc;
      int offset = inst.params[0].val.intValue;
      branchTarget.instructionIndex += offset;
      // Backward branches are only present at the unconditional branch that
      // takes the pc back to the start of the loop so we treat them differently
      if (offset < 0)
        innerBranchTargets.insert(branchTarget);
      else
        outerBranchTargets.insert(branchTarget);
      break;
    }

    case JVM::ICONST_M1... JVM::ICONST_5:
    case JVM::LCONST_0... JVM::LCONST_1:
    case JVM::FCONST_0... JVM::FCONST_2:
    case JVM::DCONST_0... JVM::DCONST_1:
    case JVM::ILOAD_0... JVM::DLOAD_3:
    case JVM::ISTORE_0... JVM::DSTORE_3: {
      inst.params.push_back(extractParam(inst.mnemonic));
      inst.mnemonic = extractMnemonic(inst.mnemonic);
    }
    default:
      break;
  }
  recordedTrace.push_back({pc, inst});
  return false;
}

const Value TraceRecorder::extractParam(const JVM::Mnemonic mnem) {
  switch (mnem) {
    case JVM::ILOAD_0:
    case JVM::FLOAD_0:
    case JVM::LLOAD_0:
    case JVM::DLOAD_0:
    case JVM::ISTORE_0:
    case JVM::FSTORE_0:
    case JVM::LSTORE_0:
    case JVM::DSTORE_0:
    case JVM::ICONST_0:
      return Value(0);
    case JVM::ILOAD_1:
    case JVM::FLOAD_1:
    case JVM::LLOAD_1:
    case JVM::DLOAD_1:
    case JVM::ISTORE_1:
    case JVM::FSTORE_1:
    case JVM::LSTORE_1:
    case JVM::DSTORE_1:
    case JVM::ICONST_1:
      return Value(1);
    case JVM::ILOAD_2:
    case JVM::FLOAD_2:
    case JVM::LLOAD_2:
    case JVM::DLOAD_2:
    case JVM::ISTORE_2:
    case JVM::FSTORE_2:
    case JVM::LSTORE_2:
    case JVM::DSTORE_2:
    case JVM::ICONST_2:
      return Value(2);
    case JVM::ILOAD_3:
    case JVM::FLOAD_3:
    case JVM::LLOAD_3:
    case JVM::DLOAD_3:
    case JVM::ISTORE_3:
    case JVM::FSTORE_3:
    case JVM::LSTORE_3:
    case JVM::DSTORE_3:
    case JVM::ICONST_3:
      return Value(3);
    case JVM::ICONST_4:
      return Value(4);
    case JVM::ICONST_5:
      return Value(5);
    case JVM::ICONST_M1:
      return Value(-1);
    case JVM::FCONST_0:
      return Value(0.0f);
    case JVM::FCONST_1:
      return Value(1.0f);
    case JVM::FCONST_2:
      return Value(2.0f);
    case JVM::LCONST_0:
      return Value(0l);
    case JVM::LCONST_1:
      return Value(1l);
    case JVM::DCONST_0:
      return Value(0.0);
    case JVM::DCONST_1:
      return Value(1.0);
    default:
      cerr << "extractParam: " << JVM::byteCodeNames.at(mnem)
           << " should not be converted " << endl;
      throw;
  }
}

const JVM::Mnemonic TraceRecorder::extractMnemonic(const JVM::Mnemonic mnem) {
  switch (mnem) {
    case JVM::ILOAD_0... JVM::ILOAD_3:
      return JVM::ILOAD;
    case JVM::FLOAD_0... JVM::FLOAD_3:
      return JVM::FLOAD;
    case JVM::LLOAD_0... JVM::LLOAD_3:
      return JVM::LLOAD;
    case JVM::DLOAD_0... JVM::DLOAD_3:
      return JVM::DLOAD;
    case JVM::ISTORE_0... JVM::ISTORE_3:
      return JVM::ISTORE;
    case JVM::FSTORE_0... JVM::FSTORE_3:
      return JVM::FSTORE;
    case JVM::LSTORE_0... JVM::LSTORE_3:
      return JVM::LSTORE;
    case JVM::DSTORE_0... JVM::DSTORE_3:
      return JVM::DSTORE;
    case JVM::ICONST_M1... JVM::ICONST_5:
    case JVM::FCONST_0... JVM::FCONST_2:
      return JVM::LDC;
    case JVM::LCONST_0... JVM::LCONST_1:
    case JVM::DCONST_0... JVM::DCONST_1:
      return JVM::LDC2_W;
    default:
      cerr << "extractMnemonic: " << JVM::byteCodeNames.at(mnem)
           << " should not be converted " << endl;
      throw;
      break;
  }
}