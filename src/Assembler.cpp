#include "Assembler.hpp"

using namespace std;
using namespace asmjit;

// Public functions

vector<uint8_t> Assembler::assemble(vector<string>& asmRows) {
  using namespace x86;
  Environment env;
  env.setArch(Environment::kArchX64);

  CodeHolder codeHolder;
  codeHolder.init(env);
  x86::Assembler asmAssembler(&codeHolder);

  for (auto asmRow : asmRows) {
    transform(asmRow.begin(), asmRow.end(), asmRow.begin(), ::toupper);
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
    x86::Inst::Id inst = lookupMnemonics.at(mnemonic);

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
      perror("Emit error");
      cout << "Compilen't" << endl;
      exit(EXIT_FAILURE);
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
  } else if (regex_match(argument.begin(), argument.end(), memReg)) {
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
      asmjit::x86::Gp reg = lookupRegisters.at(arg.val);
      return reg;
    }
    case ArgumentType::Mem: {
      // TODO: better
      string offsetStr = arg.val.substr(4, arg.val.length() - 5);
      int offset = stoi(offsetStr);
      asmjit::x86::Mem mem = asmjit::x86::qword_ptr(asmjit::x86::rbp, offset);
      return mem;
    }
    case ArgumentType::Imm: {
      asmjit::Imm imm;
      imm.setValue(stoi(arg.val));
      return imm;
    }
  }
}