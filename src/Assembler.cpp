#include "Assembler.hpp"

using namespace std;

// Public functions

vector<uint8_t> Assembler::assemble(vector<Instruction>& nativeTrace,
                                    set<ProgramCounter> branchTargets) {
  map<ProgramCounter, asmjit::Label> labels;
  asmjit::Environment env;
  env.setArch(asmjit::Environment::kArchX64);

  asmjit::CodeHolder codeHolder;
  codeHolder.init(env);
  asmjit::x86::Assembler asmAssembler(&codeHolder);
  for (auto pc : branchTargets) {
    labels[pc] = asmAssembler.newLabel();
  }
  for (auto inst : nativeTrace) {
    switch (inst.inst) {
      case x86::LABEL:
        asmAssembler.bind(labels[inst.op1.pc]);
        break;
      // Mnemonics that have 0 operands
      case x86::LEAVE:
      case x86::RET: {
        // TODO: add function to convert our operand to asmjit operand
        break;
      }
      // Mnemonics that have 1 operand
      case x86::PUSH:
      case x86::POP:
      case x86::INC:
      case x86::JMP:
      case x86::JGE: {
        break;
      }
      // Mnemonics that have 2 operands (the rest)
      default: {
        break;
      }
    }
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

static const ArgumentType getArgumentType(string argument) {
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

static const Argument getArgument(string argString) {
  ArgumentType t = getArgumentType(argString);
  Argument arg = {t, argString};
  return arg;
}

static const asmjit::Operand convert(Argument arg) {
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