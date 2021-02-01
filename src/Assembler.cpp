#include "Assembler.hpp"

using namespace std;
using namespace asmjit;

// Private functions

// Public functions
void Assembler::assemble(vector<string>& asmRows) {
  JitRuntime runtime;
  CodeHolder codeHolder;
  codeHolder.init(runtime.environment());
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
    x86::Inst::Id inst;

    if (mnemonic == "MOV") {
      inst = x86::Inst::kIdMov;
    } else if (mnemonic == "ADD") {
      inst = x86::Inst::kIdAdd;
    } else if (mnemonic == "ENTER") {
      inst = x86::Inst::kIdEnter;
    } else if (mnemonic == "LEAVE") {
      inst = x86::Inst::kIdLeave;
    } else if (mnemonic == "RET") {
      inst = x86::Inst::kIdRet;
    }

    switch (operands) {
      case 2:
        asmAssembler.emit(inst, op1, op2);
        break;
      case 1:
        asmAssembler.emit(inst, op1);
        break;
      default:
        asmAssembler.emit(inst);
        break;
    }
  }
  Func fn;
  Error err = runtime.add(&fn, &codeHolder);
  if (err) {
    cout << "Compilen't";
    perror("");
    exit(EXIT_FAILURE);
  }
  int res = fn();
  cout << "Res: " << res << endl;
  runtime.release(fn);
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

static const x86::Gp getReg(std::string regString) {
  if (regString == "RAX") {
    return x86::rax;
  } else if (regString == "RCX") {
    return x86::rcx;
  } else if (regString == "RDX") {
    return x86::rdx;
  } else if (regString == "RBX") {
    return x86::rbx;
  } else if (regString == "RSP") {
    return x86::rsp;
  } else if (regString == "RBP") {
    return x86::rbp;
  } else if (regString == "RSI") {
    return x86::rsi;
  } else if (regString == "RDI") {
    return x86::rdi;
  } else if (regString == "R8") {
    return x86::r8;
  } else if (regString == "R9") {
    return x86::r9;
  } else if (regString == "R10") {
    return x86::r10;
  } else if (regString == "R11") {
    return x86::r11;
  } else if (regString == "R12") {
    return x86::r12;
  } else if (regString == "R13") {
    return x86::r13;
  } else if (regString == "R14") {
    return x86::r14;
  } else {  // R15
    return x86::r15;
  }
}

static const Argument getArgument(string argString) {
  ArgumentType t = getArgumentType(argString);
  Argument arg = {t, argString};
  return arg;
}

static const Operand convert(Argument arg) {
  switch (arg.type) {
    case ArgumentType::Reg: {
      x86::Gp reg = getReg(arg.val);
      return reg;
    }
    case ArgumentType::Mem: {
      x86::Mem mem = x86::ptr(x86::rbp, 8);
      return mem;
    }
    case ArgumentType::Imm: {
      asmjit::Imm imm;
      imm.setValue(stoi(arg.val));
      return imm;
    }
  }
}