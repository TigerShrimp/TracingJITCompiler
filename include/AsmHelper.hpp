#ifndef ASM_HELPER_HPP
#define ASM_HELPER_HPP

#include <asmjit/x86.h>

#include <map>
#include <string>

#include "Definitions.hpp"

typedef std::map<std::string, asmjit::x86::Reg> regMap;
typedef std::map<std::string, asmjit::x86::Inst::Id> mnemMap;

const regMap lookupRegisters = {
    {"RAX", asmjit::x86::rax},   {"RCX", asmjit::x86::rcx},
    {"RDX", asmjit::x86::rdx},   {"RBX", asmjit::x86::rbx},
    {"RSP", asmjit::x86::rsp},   {"RBP", asmjit::x86::rbp},
    {"RSI", asmjit::x86::rsi},   {"RDI", asmjit::x86::rdi},
    {"R8", asmjit::x86::r8},     {"R9", asmjit::x86::r9},
    {"R10", asmjit::x86::r10},   {"R11", asmjit::x86::r11},
    {"R12", asmjit::x86::r12},   {"R13", asmjit::x86::r13},
    {"R14", asmjit::x86::r14},   {"R15", asmjit::x86::r15},
    {"XMM0", asmjit::x86::xmm0}, {"XMM1", asmjit::x86::xmm1}};

const mnemMap lookupMnemonics = {{"MOV", asmjit::x86::Inst::kIdMov},
                                 {"ADD", asmjit::x86::Inst::kIdAdd},
                                 {"ENTER", asmjit::x86::Inst::kIdEnter},
                                 {"LEAVE", asmjit::x86::Inst::kIdLeave},
                                 {"RET", asmjit::x86::Inst::kIdRet},
                                 {"IMUL", asmjit::x86::Inst::kIdImul},
                                 {"SUB", asmjit::x86::Inst::kIdSub},
                                 {"PUSH", asmjit::x86::Inst::kIdPush},
                                 {"POP", asmjit::x86::Inst::kIdPop},
                                 {"INC", asmjit::x86::Inst::kIdInc},
                                 {"MOVQ", asmjit::x86::Inst::kIdMovq},
                                 {"ADDSD", asmjit::x86::Inst::kIdAddsd},
                                 {"MULSD", asmjit::x86::Inst::kIdMulsd},
                                 {"SUBSD", asmjit::x86::Inst::kIdSubsd}};

#endif  // ASM_HELPER_HPP