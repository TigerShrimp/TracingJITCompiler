#include "Compiler.hpp"

using namespace std;

CompiledTrace Compiler::compile(Recording recording) {
  DEBUG_PRINT("Compiling starting\n");
  resetCompilerState();

  DEBUG_PRINT("Compiling trace starting\n");
  for (auto entry : recording.recordedTrace) {
    DEBUG_PRINT("Compiling {}\n", JVM::byteCodeNames.at(entry.inst.mnemonic));
    compile(entry, recording.innerBranchTargets.contains(entry.pc));
  }

  DEBUG_PRINT("Compiling init starting\n");
  // Code that is run before the trace starts to move variables from local
  // variable store into usable registers for trace execution
  // TODO make enter correct, needs offset to allign memory
  vector<Instruction> initCode;
  initCode.push_back(
      {x86::ENTER, {IMMEDIATE, .val = Value(0)}, {IMMEDIATE, .val = Value(0)}});
  for (const auto& [reg, var] : regInitTable) {
    Op dst = {REGISTER, .reg = reg};
    initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {RDI, 8 * (int)var}}});
    initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {reg, 0}}});
  }

  DEBUG_PRINT("Compiling bailout starting\n");
  // Code run after trace is finished, it is the same for default exit at the
  // end of the trace and for side exits. It writes the values back into the
  // local variable store, all values are written to and from the trace even if
  // they are not used, it is simpler this way.
  vector<Instruction> bailoutCode;
  map<size_t, ProgramCounter> exitPoints;
  int id = 0;
  ProgramCounter bailoutPc = {0, 0};
  Op bailoutLabel = {LABEL, .pc = bailoutPc};
  for (const auto& pc : recording.outerBranchTargets) {
    // Label to jump to when exiting.
    bailoutCode.push_back({x86::LABEL, labelAt(pc, Value(0))});
    bailoutCode.push_back(
        {x86::MOV, {REGISTER, .reg = RAX}, {IMMEDIATE, .val = Value(id)}});
    bailoutCode.push_back({x86::JMP, bailoutLabel});
    DEBUG_PRINT("outer branch target: ({},{})\n", pc.methodIndex,
                pc.instructionIndex);
    exitPoints[id++] = pc;
  }
  Op rdi = {REGISTER, .reg = RDI};
  Op rdiPtr = {MEMORY, .mem = {RDI, 0}};

  bailoutCode.push_back({x86::LABEL, bailoutLabel});
  for (const auto& [var, op] : variableTable) {
    // Since we cannot be sure there is nothing taking up space in RDI, the
    // register used for holding the local variables, we must push and pop
    // before and after fetching each variable.
    bailoutCode.push_back({x86::PUSH, rdi});
    bailoutCode.push_back(
        {x86::MOV, rdi, {MEMORY, .mem = {RDI, 8 * (int)var}}});
    bailoutCode.push_back({x86::MOV, rdiPtr, op});
    bailoutCode.push_back({x86::POP, rdi});
  }
  bailoutCode.push_back(
      {x86::MOV, {REGISTER, .reg = RAX}, {IMMEDIATE, .val = Value(0)}});
  bailoutCode.push_back({x86::LEAVE});
  bailoutCode.push_back({x86::RET});
  // Combine all branch targets into a single set.

  vector<ProgramCounter> branchTargets;
  for (auto pc : recording.innerBranchTargets) branchTargets.push_back(pc);
  for (auto pc : recording.outerBranchTargets) branchTargets.push_back(pc);
  branchTargets.push_back(bailoutPc);
  vector<uint8_t> assembledCode =
      assembler.assemble(initCode, nativeTrace, bailoutCode, branchTargets);
  return {exitPoints, assembledCode};
}

void Compiler::resetCompilerState() {
  nativeTrace.clear();
  variableTable.clear();
  while (!operandStack.empty()) operandStack.pop();
  // TODO: fill avail regs and xregs with all registers
  while (!availableRegs.empty()) availableRegs.pop();
  availableRegs.push(RSI);
  availableRegs.push(RDX);
  availableRegs.push(RCX);
  availableRegs.push(R8);
  availableRegs.push(R9);
}

void Compiler::compile(RecordEntry entry, bool startsWithLabel) {
  if (startsWithLabel)
    nativeTrace.push_back({x86::LABEL, labelAt(entry.pc, Value(0))});
  switch (entry.inst.mnemonic) {
    // TODO: Change ILOAD_X to regular ILOAD with parameter. Same with
    // store/const.
    case JVM::ILOAD_1: {
      // Check variableTable
      if (!variableTable.contains(1)) {
        // If not exist:
        //     assign to register and place in variable table
        placeInNextAvailableRegister(1, Int);
      }
      // push op to operand stack
      operandStack.push(variableTable[1]);
      break;
    }
    case JVM::ILOAD_2: {
      if (!variableTable.contains(2)) {
        placeInNextAvailableRegister(2, Int);
      }
      operandStack.push(variableTable[2]);
      break;
    }
    case JVM::ILOAD_3: {
      if (!variableTable.contains(3)) {
        placeInNextAvailableRegister(3, Int);
      }
      operandStack.push(variableTable[3]);
      break;
    }
    case JVM::ISTORE_3: {
      Op op = operandStack.top();
      operandStack.pop();
      switch (op.opType) {
        case REGISTER:
        case MEMORY:
          variableTable[3] = op;
          break;
        case IMMEDIATE: {
          if (!variableTable.contains(3)) {
            placeInNextAvailableRegister(3, Int);
          }
          nativeTrace.push_back({x86::MOV, variableTable[3], op});
          break;
        }
        default: {
          cerr << "int in't X register" << endl;
          throw;
        }
      }
      break;
    }
    case JVM::SIPUSH: {
      operandStack.push({IMMEDIATE, .val = entry.inst.params[0]});
      break;
    }
    case JVM::IF_ICMPGE: {
      Op op2 = operandStack.top();
      operandStack.pop();
      Op op1 = operandStack.top();
      operandStack.pop();
      if (op1.opType == REGISTER || op1.opType == MEMORY) {
        nativeTrace.push_back({x86::CMP, op1, op2});
        nativeTrace.push_back(
            {x86::JGE, labelAt(entry.pc, entry.inst.params[0])});
      } else {
        cerr << "Handle move to reg before operation" << endl;
        throw;
      }
      break;
    }

    case JVM::GOTO:
      nativeTrace.push_back(
          {x86::JMP, labelAt(entry.pc, entry.inst.params[0])});
      break;
    case JVM::IADD: {
      Op op2 = operandStack.top();
      operandStack.pop();
      Op op1 = operandStack.top();
      operandStack.pop();
      Op opDst = getFirstAvailableReg();
      nativeTrace.push_back({x86::MOV, opDst, op1});
      nativeTrace.push_back({x86::ADD, opDst, op2});
      operandStack.push(opDst);
      break;
    }
    case JVM::IINC: {
      size_t varIndex = entry.inst.params[0].val.intValue;
      if (!variableTable.contains(varIndex)) {
        placeInNextAvailableRegister(varIndex, Int);
      }
      nativeTrace.push_back({x86::ADD,
                             variableTable[varIndex],
                             {IMMEDIATE, .val = entry.inst.params[1]}});
      break;
    }
    default:
      break;
  }
}

void Compiler::placeInNextAvailableRegister(size_t var, BaseType type) {
  switch (type) {
    case Int:
    case Long: {
      // Hittar ett ledigt register.
      // Om det är ledigt från början -> add to init map (reg->var);
      // Om det inte är det -> se till init flyttar var till stack frame
      //   och här gör någon slags mov hit
      Op reg = getFirstAvailableReg();
      regInitTable[reg.reg] = var;
      variableTable[var] = reg;
      break;
    }
    case Float:
    case Double: {
      if (!availableXRegs.empty()) {
        XREG xreg = availableXRegs.top();
        availableXRegs.pop();
        variableTable[var] = {XMM_REGISTER, .xreg = xreg};
      } else {
        cerr << "Can not handle non-free xmm registers yet either" << endl;
        throw;
      }
    }
    default:
      break;
  }
}

Op Compiler::getFirstAvailableReg() {
  if (!availableRegs.empty()) {
    REG reg = availableRegs.top();
    availableRegs.pop();
    return {REGISTER, .reg = reg};
  } else {
    cerr << "Can not handle non-free registers yet" << endl;
    throw;
  }
}

Op Compiler::labelAt(ProgramCounter pc, Value offset) {
  int intOffset = offset.val.intValue;
  size_t labelPosition = pc.instructionIndex + intOffset;
  return {LABEL, .pc = {labelPosition, pc.methodIndex}};
}