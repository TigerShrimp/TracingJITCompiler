#include "Compiler.hpp"

using namespace std;

Compiler::~Compiler() { memoryHandler.freeTraces(); }

Trace Compiler::compileAndInstall(size_t maxLocals, Recording recording) {
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
  initCode.push_back({x86::ENTER,
                      {IMMEDIATE, .val = Value(16)},
                      {IMMEDIATE, .val = Value(0)}});
  for (const auto& [dst, var] : initTable) {
    initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {RDI, 8 * (int)var}}});
    initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {dst.reg, 0}}});
  }

  DEBUG_PRINT("Compiling bailout starting\n");
  // Code run after trace is finished, it is the same for default exit at the
  // end of the trace and for side exits. It writes the values back into the
  // local variable store, all values are written to and from the trace even if
  // they are not used, it is simpler this way.
  Op rdi = {REGISTER, .reg = RDI};
  Op rdiPtr = {MEMORY, .mem = {RDI, 0}};

  bailoutCode.push_back({x86::LABEL, exitLabel});
  for (const auto& [op, var] : initTable) {
    // TODO: Maybe change this to be a little more optimized
    // Since we cannot be sure there is nothing taking up space in RDI, the
    // register used for holding the local variables, we must push and pop
    // before and after fetching each variable.
    bailoutCode.push_back({x86::PUSH, rdi});
    bailoutCode.push_back(
        {x86::MOV, rdi, {MEMORY, .mem = {RDI, 8 * (int)var}}});
    bailoutCode.push_back({x86::MOV, rdiPtr, op});
    bailoutCode.push_back({x86::POP, rdi});
  }
  bailoutCode.push_back({x86::LEAVE});
  bailoutCode.push_back({x86::RET});
  // Combine all branch targets into a single set.

  vector<ProgramCounter> branchTargets;
  for (auto pc : recording.innerBranchTargets) branchTargets.push_back(pc);
  for (auto pc : recording.outerBranchTargets) branchTargets.push_back(pc);
  branchTargets.push_back(exitPc);
  vector<uint8_t> assembledCode =
      assembler.assemble(initCode, nativeTrace, bailoutCode, branchTargets);
  TracePointer ptr = memoryHandler.writeTrace(assembledCode);
  return {ptr, exitPoints, maxLocals};
}

void Compiler::resetCompilerState() {
  nativeTrace.clear();
  initTable.clear();
  bailoutCode.clear();
  exitPoints.clear();
  variableTable.clear();
  exitId = 0;
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
    // ILOAD, ICONST and ISTORE have been changed from ILOAD_0, ILOAD_1 etc. to
    // their parameterized counter parts in order to reduce code duplication
    case JVM::ILOAD: {
      int var = entry.inst.params[0].val.intValue;
      // Check variableTable
      if (!variableTable.contains(var)) {
        // If not exist:
        //     assign to register and place in variable table
        placeInNextAvailableRegister(var, Int);
      }
      // push op to operand stack
      operandStack.push(variableTable[var]);
      break;
    }
    case JVM::ISTORE: {
      int var = entry.inst.params[0].val.intValue;
      Op op = operandStack.top();
      operandStack.pop();
      switch (op.opType) {
        case REGISTER:
          // We do this in a more convoluted way to make sure that the variable
          // switches registers so that we can test the functionality of the
          // bailout code.
          if (variableTable.contains(var) &&
              variableTable[var].opType == REGISTER) {
            availableRegs.push(variableTable[var].reg);
            variableTable[var] = op;
            break;
          } else {
            availableRegs.push(op.reg);
            // Intentional fallthrough
          }
        case MEMORY:
        case IMMEDIATE: {
          if (!variableTable.contains(var)) {
            placeInNextAvailableRegister(var, Int);
          }
          concat(nativeTrace, generateMov(variableTable[var], op));
          break;
        }
        default: {
          cerr << "int in't X register" << endl;
          throw;
        }
      }
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
    case JVM::IF_ICMPLE: {
      concat(nativeTrace,
             generateCondBranch(x86::JLE,
                                labelAt(entry.pc, entry.inst.params[0])));
      break;
    }
    case JVM::GOTO:
      concat(nativeTrace, restoreInitState());
      nativeTrace.push_back(
          {x86::JMP, labelAt(entry.pc, entry.inst.params[0])});
      break;
    case JVM::IADD: {
      concat(nativeTrace, generateArithmetic(x86::ADD));
      break;
    }
    case JVM::ISUB: {
      concat(nativeTrace, generateArithmetic(x86::SUB));

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
      cerr << "Compilen't " << JVM::byteCodeNames.at(entry.inst.mnemonic)
           << " yet" << endl;
      throw;
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
      initTable[reg] = var;
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

void Compiler::compileBailoutFor(Op label) {
  bailoutCode.push_back({x86::LABEL, label});
  concat(bailoutCode, restoreInitState());
  long idForPc = exitId++;
  exitPoints[idForPc] = label.pc;
  bailoutCode.push_back(
      {x86::MOV, {REGISTER, .reg = RAX}, {IMMEDIATE, .val = Value(idForPc)}});
  bailoutCode.push_back({x86::JMP, exitLabel});
}

vector<Instruction> Compiler::restoreInitState() {
  vector<Instruction> instructions;
  map<size_t, Op> variableTableCopy(variableTable);
  for (const auto& [op, var] : initTable) {
    if (op != variableTableCopy[var]) {
      if (op.opType == REGISTER || op.opType == MEMORY) {
        concat(instructions, movWithSwap(variableTableCopy, op, var));
      }
    }
    // Op dst = {REGISTER, .reg = reg};
    // initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {RDI, 8 *
    // (int)var}}}); initCode.push_back({x86::MOV, dst, {MEMORY, .mem = {reg,
    // 0}}});
  }
  return instructions;
}

vector<Instruction> Compiler::movWithSwap(map<size_t, Op>& currVariableTable,
                                          Op dst, size_t var) {
  vector<Instruction> instructions;
  for (const auto& [var1, op1] : currVariableTable) {
    if (dst == op1) {
      instructions.push_back({x86::PUSH, op1});
      concat(instructions, generateMov(dst, currVariableTable[var]));
      instructions.push_back({x86::POP, currVariableTable[var]});
      currVariableTable[var1] = currVariableTable[var];
      currVariableTable[var] = dst;
      return instructions;
    }
  }
  return generateMov(dst, currVariableTable[var]);
}

vector<Instruction> Compiler::generateMov(Op dst, Op src) {
  vector<Instruction> instructions;
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

vector<Instruction> Compiler::generateCondBranch(x86::Mnemonic instr,
                                                 Op label) {
  vector<Instruction> instructions;
  Op op2 = operandStack.top();
  operandStack.pop();
  Op op1 = operandStack.top();
  operandStack.pop();
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

vector<Instruction> Compiler::generateArithmetic(x86::Mnemonic inst) {
  vector<Instruction> instructions;
  Op op2 = operandStack.top();
  operandStack.pop();
  Op op1 = operandStack.top();
  operandStack.pop();
  Op opDst = getFirstAvailableReg();
  concat(instructions, generateMov(opDst, op1));
  instructions.push_back({inst, opDst, op2});
  operandStack.push(opDst);
  return instructions;
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

// Extend the first list with the elements of the second list.
inline void concat(vector<Instruction>& first, vector<Instruction> second) {
  for (auto inst : second) first.push_back(inst);
}