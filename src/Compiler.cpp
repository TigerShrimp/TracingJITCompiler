#include "Compiler.hpp"

using namespace std;

vector<Instruction> Compiler::compile(vector<RecordEntry> recordedTrace) {
  resetCompilerState();
  for (auto entry : recordedTrace) {
    compile(entry);
  }
  return nativeTrace;
}

void Compiler::resetCompilerState() {
  nativeTrace.clear();
  variableTable.clear();
  while (!operandStack.empty()) operandStack.pop();
  // TODO: fill avail regs and xregs with all registers
}

void Compiler::compile(RecordEntry entry) {
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
        // TODO: nativeTrace.push_back({x86::JGE, bailoutLabel})
      } else {
        cerr << "Handle move to reg before operation" << endl;
        throw;
      }
      break;
    }

    case JVM::GOTO:
      // TODO: nativeTrace.push_back({x86::JMP, startLabel...})
    case JVM::IADD: {
      Op op2 = operandStack.top();
      operandStack.pop();
      Op op1 = operandStack.top();
      operandStack.pop();
      Op opDst = getFirstAvailableReg();
      nativeTrace.push_back({x86::MOV, opDst, op1});
      nativeTrace.push_back({x86::ADD, opDst, op2});
      operandStack.push(opDst);
    }
    case JVM::IINC:
    default:
      break;
  }
}

void Compiler::placeInNextAvailableRegister(size_t var, BaseType type) {
  switch (type) {
    case Int:
    case Long: {
      variableTable[var] = getFirstAvailableReg();
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

Op getFirstAvailableReg() {
  if (!availableRegs.empty()) {
    REG reg = availableRegs.top();
    availableRegs.pop();
    return {REGISTER, .reg = reg};
  } else {
    cerr << "Can not handle non-free registers yet" << endl;
    throw;
  }
}