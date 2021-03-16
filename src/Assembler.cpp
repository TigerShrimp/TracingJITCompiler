#include "Assembler.hpp"

using namespace std;

// Public functions

vector<uint8_t> Assembler::assemble(vector<Instruction>& initCode,
                                    vector<Instruction>& nativeTrace,
                                    vector<Instruction>& bailout,
                                    vector<ProgramCounter> branchTargets) {
  DEBUG_PRINT("Assemble start\n");
  asmjit::Environment env;
  env.setArch(asmjit::Environment::kArchX64);

  asmjit::CodeHolder codeHolder;
  codeHolder.init(env);
  asmjit::x86::Assembler asmAssembler(&codeHolder);

  labels.clear();
  for (auto pc : branchTargets) {
    DEBUG_PRINT("Label at = ({},{})\n", pc.methodIndex, pc.instructionIndex);
    labels[pc] = asmAssembler.newLabel();
  }

  assemble(asmAssembler, initCode);
  DEBUG_PRINT("Assemble nativeTrace\n");
  assemble(asmAssembler, nativeTrace);
  DEBUG_PRINT("Assemble bailout\n");
  assemble(asmAssembler, bailout);

  asmjit::CodeBuffer& buffer = codeHolder.textSection()->buffer();
  vector<uint8_t> bytes(buffer.data(), buffer.data() + buffer.size());

  return bytes;
}

void Assembler::assemble(asmjit::x86::Assembler& asmAssembler,
                         std::vector<Instruction>& insts) {
  asmjit::Error err;
  for (auto inst : insts) {
    switch (inst.inst) {
      case x86::LABEL:
        DEBUG_PRINT("Binding label ({},{})\n", inst.op1.pc.methodIndex,
                    inst.op1.pc.instructionIndex)
        err = asmAssembler.bind(labels[inst.op1.pc]);
        break;
      // Mnemonics that have 0 operands
      case x86::LEAVE:
      case x86::RET: {
        err = asmAssembler.emit(lookupX86Mnemonics.at(inst.inst));
        break;
      }
      // Mnemonics that have 1 operand
      case x86::PUSH:
      case x86::POP:
      case x86::INC:
      case x86::JMP:
      case x86::JGE: {
        asmjit::Operand op = convert(inst.op1);
        err = asmAssembler.emit(lookupX86Mnemonics.at(inst.inst), op);
        break;
      }
      // Mnemonics that have 2 operands (the rest)
      default: {
        asmjit::Operand op1 = convert(inst.op1);
        asmjit::Operand op2 = convert(inst.op2);
        err = asmAssembler.emit(lookupX86Mnemonics.at(inst.inst), op1, op2);
        break;
      }
    }
    if (err) {
      cerr << "Emit error: " << asmjit::DebugUtils::errorAsString(err) << " "
           << inst.inst << " " << inst.op1.opType << endl;
      throw;
    }
  }
}

asmjit::Operand Assembler::convert(Op op) {
  switch (op.opType) {
    case REGISTER:
      return lookupX86Registers.at(op.reg);
    case XMM_REGISTER:
      return lookupXmmRegisters.at(op.xreg);
    case MEMORY: {
      asmjit::x86::Gp base = lookupX86Registers.at(op.mem.reg);
      asmjit::x86::Mem mem = asmjit::x86::qword_ptr(base, op.mem.offset);
      return mem;
    }
    case IMMEDIATE: {
      asmjit::Imm imm;
      switch (op.val.type.type) {
        case Int:
          imm.setValue(op.val.val.intValue);
          break;
        case Float:
          imm.setValue(op.val.val.floatValue);
          break;
        case Long:
          imm.setValue(op.val.val.longValue);
          break;
        case Double:
          imm.setValue(op.val.val.doubleValue);
          break;
        default:
          break;
      }
      return imm;
    }
    case LABEL:
      return labels[op.pc];
  }
}

vector<uint8_t> Assembler::assemble(vector<string>& asmRows) {
  using namespace asmjit;
  Environment env;
  env.setArch(Environment::kArchX64);

  CodeHolder codeHolder;
  codeHolder.init(env);
  asmjit::x86::Assembler asmAssembler(&codeHolder);

  for (auto asmRow : asmRows) {
    transform(asmRow.begin(), asmRow.end(), asmRow.begin(), ::toupper);
    size_t commentStart = asmRow.find(';');
    if (commentStart == 0)
      continue;
    else if (commentStart != string::npos) {
      asmRow = asmRow.substr(0, commentStart);
    }
    replace(asmRow.begin(), asmRow.end(), ',', ' ');
    vector<string> instParts;
    string part;
    stringstream ss(asmRow);
    while (getline(ss, part, ' ')) {
      if (part.size() != 0) {
        instParts.push_back(part);
      }
    }
    int operands = instParts.size() - 1;

    Operand op1, op2;
    switch (operands) {
      case 2:
        op2 = convert(getArgument(instParts[2]));
      case 1:
        op1 = convert(getArgument(instParts[1]));
        break;
    }
    string mnemonic = instParts[0];
    asmjit::x86::Inst::Id inst = lookupMnemonics.at(mnemonic);

    Error err;
    switch (operands) {
      case 2:
        err = asmAssembler.emit(inst, op1, op2);
        break;
      case 1:
        err = asmAssembler.emit(inst, op1);
        break;
      default:
        err = asmAssembler.emit(inst);
        break;
    }
    if (err) {
      cerr << "Emit error: " << DebugUtils::errorAsString(err) << endl;
      throw;
    }
  }
  CodeBuffer& buffer = codeHolder.textSection()->buffer();
  vector<uint8_t> bytes(buffer.data(), buffer.data() + buffer.size());

  return bytes;
}

ArgumentType Assembler::getArgumentType(string argument) {
  ArgumentType arg;
  if (regex_match(argument.begin(), argument.end(), immReg)) {
    arg = ArgumentType::Imm;
  } else if (regex_match(argument.begin(), argument.end(), ptrReg)) {
    arg = ArgumentType::Mem;
  } else if (regex_match(argument.begin(), argument.end(), regReg)) {
    arg = ArgumentType::Reg;
  } else {
    throw;
  }
  return arg;
}

Argument Assembler::getArgument(string argString) {
  ArgumentType t = getArgumentType(argString);
  Argument arg = {t, argString};
  return arg;
}

asmjit::Operand Assembler::convert(Argument arg) {
  switch (arg.type) {
    case ArgumentType::Reg: {
      asmjit::x86::Reg reg = lookupRegisters.at(arg.val);
      return reg;
    }
    case ArgumentType::Mem: {
      smatch matches;
      regex_search(arg.val, matches, ptrReg);
      asmjit::x86::Gp base =
          lookupRegisters.at(matches[1]).as<asmjit::x86::Gp>();
      int offset = matches[2] == "" ? 0 : stoi(matches[2]);
      asmjit::x86::Mem mem = asmjit::x86::qword_ptr(base, offset);
      return mem;
    }
    case ArgumentType::Imm: {
      asmjit::Imm imm;
      if (arg.val.find('.') != string::npos) {
        imm.setValue(stod(arg.val));
      } else {
        imm.setValue(stoi(arg.val));
      }
      return imm;
    }
  }
}