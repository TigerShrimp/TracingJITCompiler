#include "Compiler.hpp"

using namespace std;

Compiler::~Compiler() { memoryHandler.freeTraces(); }

Trace Compiler::compileAndInstall(int maxLocals, Recording recording) {
  DEBUG_PRINT("Compiling starting\n");
  resetCompilerState();
  DEBUG_PRINT("Inner branchTargets: {}\n", recording.innerBranchTargets.size());
  DEBUG_PRINT("Compiling trace starting\n");
  for (auto entry : recording.recordedTrace) {
    DEBUG_PRINT("Compiling {}\n", JVM::byteCodeNames.at(entry.inst.mnemonic));
    compile(entry, recording.innerBranchTargets);
  }

  DEBUG_PRINT("Compiling init starting\n");
  // Code that is run before the trace starts to move variables from local
  // variable store into usable registers for trace execution

  list<Instruction> initCode;
  // We allocate space as if all local variables might be in memory at the same
  // time even though they might not be. This is a trade-off between memory
  // efficiency and simplicity of the code.

  // TODO: change explicit 8 to variable "sizeOfRegister".
  initCode.push_back(
      {x86::ENTER, {IMMEDIATE, .val = Value(0)}, {IMMEDIATE, .val = Value(0)}});

  Op rdi = {REGISTER, .reg = RDI};
  Op rsi = {REGISTER, .reg = RSI};
  Op rax = {REGISTER, .reg = RAX};
  Op rdiPtr = {MEMORY, .mem = {RDI, 0}};
  initCode.push_back({x86::PUSH, rdi});
  initCode.push_back({x86::PUSH, rsi});
  initCode.push_back({x86::MOV, rdi, rdiPtr});

  DEBUG_PRINT("Compiling bailout starting\n");
  // Code run after trace is finished, it is the same for default exit at the
  // end of the trace and for side exits. It writes the values back into the
  // local variable store, all values are written to and from the trace even
  // if they are not used, it is simpler this way.

  bailoutCode.push_back({x86::LABEL, exitLabel});
  bailoutCode.push_back({x86::POP, rax});
  bailoutCode.push_back({x86::POP, rdi});
  bailoutCode.push_back({x86::LEAVE});
  bailoutCode.push_back({x86::JMP, rax});

  // Combine all parts of native code to single list
  for (auto code = initCode.rbegin(); code != initCode.rend(); code++) {
    nativeTrace.push_front(*code);
  }
  for (auto code : bailoutCode) {
    nativeTrace.push_back(code);
  }

  // Combine all branch targets into a single set.
  vector<ProgramCounter> branchTargets;
  for (auto pc : recording.innerBranchTargets) branchTargets.push_back(pc);
  for (auto pc : recording.outerBranchTargets) branchTargets.push_back(pc);
  branchTargets.push_back(exitPc);
  vector<uint8_t> assembledCode =
      assembler.assemble(nativeTrace, branchTargets);
  TracePointer ptr = memoryHandler.writeTrace(assembledCode);
  return {ptr, exitPoints, maxLocals};
}

void Compiler::resetCompilerState() {
  nativeTrace.clear();
  bailoutCode.clear();
  exitPoints.clear();
  // TODO: Maybe include RDI and RAX
  static const vector<REG> regs = {RSI, RCX, R8,  R9,  R10, R11,
                                   RBX, R12, R13, R14, R15};
  while (!operandStack.empty()) operandStack.pop();
  // TODO: fill avail regs and xregs with all registers
  while (!availableRegs.empty()) availableRegs.pop();
  for (REG r : regs) availableRegs.push(r);
}

void Compiler::compile(RecordEntry entry, set<ProgramCounter> innerLabels) {
  if (innerLabels.contains(entry.pc))
    nativeTrace.push_back({x86::LABEL, labelAt(entry.pc, Value(0))});
  switch (entry.inst.mnemonic) {
    // ILOAD, ICONST and ISTORE have been changed from ILOAD_0, ILOAD_1 etc.
    // to their parameterized counter parts in order to reduce code
    // duplication
    case JVM::ILOAD: {
      int var = entry.inst.params[0].val.intValue;
      Op dst = getFirstAvailableReg();
      concat(nativeTrace, generateVariableLoad(dst, var));
      operandStack.push(dst);
      break;
    }
    case JVM::ISTORE: {
      int var = entry.inst.params[0].val.intValue;
      Op op = popAndFree();
      concat(nativeTrace, generateVariableStore(op, var));
      break;
    }
    case JVM::BIPUSH:
    case JVM::SIPUSH:
    case JVM::LDC: {
      operandStack.push({IMMEDIATE, .val = entry.inst.params[0]});
      break;
    }
    case JVM::IF_ICMPEQ: {
      concat(
          nativeTrace,
          generateCondBranch(x86::JE, labelAt(entry.pc, entry.inst.params[0])));
      break;
    }
    case JVM::IF_ICMPGE: {
      concat(nativeTrace,
             generateCondBranch(x86::JGE,
                                labelAt(entry.pc, entry.inst.params[0])));
      break;
    }
    case JVM::IF_ICMPGT: {
      concat(
          nativeTrace,
          generateCondBranch(x86::JG, labelAt(entry.pc, entry.inst.params[0])));
      break;
    }
    case JVM::IF_ICMPLE: {
      concat(nativeTrace,
             generateCondBranch(x86::JLE,
                                labelAt(entry.pc, entry.inst.params[0])));
      break;
    }
    case JVM::IFEQ: {
      Op op = popAndFree();
      nativeTrace.push_back({x86::CMP, op, {IMMEDIATE, .val = Value(0)}});
      Op label = labelAt(entry.pc, entry.inst.params[0]);
      nativeTrace.push_back({x86::JE, label});
      compileBailoutFor(label);
      break;
    }
    case JVM::GOTO: {
      Op label = labelAt(entry.pc, entry.inst.params[0]);
      nativeTrace.push_back({x86::JMP, label});
      if (!innerLabels.contains(label.pc)) {
        compileBailoutFor(label);
      }
      break;
    }

    case JVM::IADD: {
      concat(nativeTrace, generateArithmetic(x86::ADD));
      break;
    }
    case JVM::ISUB: {
      concat(nativeTrace, generateArithmetic(x86::SUB));

      break;
    }
    case JVM::IREM: {
      Op denomOp = operandStack.top();
      operandStack.pop();
      Op nomOp = popAndFree();
      Op opDst;
      concat(nativeTrace, generateMov({REGISTER, .reg = RAX}, nomOp));
      if (denomOp.opType == REGISTER) {
        opDst = denomOp;
      } else {
        opDst = getFirstAvailableReg();
        concat(nativeTrace, generateMov(opDst, denomOp));
      }
      Op rdx = {REGISTER, .reg = RDX};
      nativeTrace.push_back({x86::MOV, rdx, {IMMEDIATE, .val = Value(0)}});
      nativeTrace.push_back({x86::IDIV, opDst});
      concat(nativeTrace, generateMov(opDst, rdx));
      operandStack.push(opDst);
      break;
    }
    case JVM::IINC: {
      int var = entry.inst.params[0].val.intValue;
      // Op dst = getFirstAvailableReg();
      // concat(nativeTrace, generateMov(dst, {MEMORY, .mem = {RDI, 8 * var}}));
      // Op dstPtr = {MEMORY, .mem = {dst.reg, 0}};
      nativeTrace.push_back({x86::ADD,
                             {MEMORY, .mem = {RDI, 8 * var}},
                             {IMMEDIATE, .val = entry.inst.params[1]}});
      // availableRegs.push(dst.reg);
      break;
    }
    default:
      cerr << "Compilen't " << JVM::byteCodeNames.at(entry.inst.mnemonic)
           << " yet" << endl;
      throw;
  }
}

Op Compiler::popAndFree() {
  Op op = operandStack.top();
  operandStack.pop();
  switch (op.opType) {
    case REGISTER:
      availableRegs.push(op.reg);
      break;
    case XMM_REGISTER:
      availableXRegs.push(op.xreg);
      break;
    default:
      break;
  }
  return op;
}

Op Compiler::getFirstAvailableReg() {
  Op reg;
  if (!availableRegs.empty()) {
    reg = {REGISTER, .reg = availableRegs.front()};
    availableRegs.pop();
  } else {
    cerr << "getFirstAvailableReg: else clause" << endl;
    throw;
  }
  return reg;
}

void Compiler::compileBailoutFor(Op label) {
  if (operandStack.size() != 0) {
    cerr << "Operand stack not empty upon leaving trace" << endl;
    throw;
  }
  bailoutCode.push_back({x86::LABEL, label});
  long idForPc = exitId++;
  exitPoints[idForPc] = label.pc;
  bailoutCode.push_back(
      {x86::MOV, {REGISTER, .reg = RSI}, {IMMEDIATE, .val = Value(idForPc)}});
  bailoutCode.push_back({x86::JMP, exitLabel});
}

list<Instruction> Compiler::generateMov(Op dst, Op src) {
  list<Instruction> instructions;
  if (dst == src) {
    return instructions;
  }
  if (dst.opType == REGISTER ||
      dst.opType == MEMORY &&
          (src.opType == IMMEDIATE || src.opType == REGISTER)) {
    instructions.push_back({x86::MOV, dst, src});
  } else if (dst.opType == MEMORY && src.opType == MEMORY) {
    instructions.push_back({x86::PUSH, src});
    instructions.push_back({x86::POP, dst});
  } else {
    cerr << "generateMove: handle combination dst: " << dst.opType
         << " src: " << src.opType << endl;
    throw;
  }
  return instructions;
}

list<Instruction> Compiler::generateVariableLoad(Op dst, int var) {
  list<Instruction> instructions;
  concat(instructions, generateMov(dst, {MEMORY, .mem = {RDI, 8 * var}}));
  return instructions;
}

list<Instruction> Compiler::generateVariableStore(Op src, int var) {
  list<Instruction> instructions;
  concat(instructions, generateMov({MEMORY, .mem = {RDI, 8 * var}}, src));
  return instructions;
}

list<Instruction> Compiler::generateCondBranch(x86::Mnemonic instr, Op label) {
  list<Instruction> instructions;
  Op op2 = popAndFree();
  Op op1 = popAndFree();
  if (op1.opType == REGISTER || op1.opType == MEMORY) {
    instructions.push_back({x86::CMP, op1, op2});
    instructions.push_back({instr, label});
    compileBailoutFor(label);
  } else {
    cerr << "Handle move to reg before operation" << endl;
    throw;
  }
  return instructions;
}

list<Instruction> Compiler::generateArithmetic(x86::Mnemonic inst) {
  list<Instruction> instructions;
  Op op2 = operandStack.top();
  operandStack.pop();
  Op op1 = operandStack.top();
  operandStack.pop();
  Op opDst;
  if (op1.opType == REGISTER) {
    opDst = op1;
  } else {
    opDst = getFirstAvailableReg();
    concat(instructions, generateMov(opDst, op1));
  }
  if (op2.opType == REGISTER) {
    availableRegs.push(op2.reg);
  }
  instructions.push_back({inst, opDst, op2});
  operandStack.push(opDst);
  return instructions;
}

Op Compiler::labelAt(ProgramCounter pc, Value offset) {
  int intOffset = offset.val.intValue;
  size_t labelPosition = pc.instructionIndex + intOffset;
  return {LABEL, .pc = {labelPosition, pc.methodIndex}};
}

// Extend the first list with the elements of the second list.
inline void concat(list<Instruction>& first, list<Instruction> second) {
  for (auto inst : second) first.push_back(inst);
}