#ifndef ASM_HELPER_HPP
#define ASM_HELPER_HPP

#include <asmjit/x86.h>

#include <map>
#include <string>

#include "Definitions.hpp"
#include "x86.hpp"
typedef std::map<std::string, asmjit::x86::Reg> regMap;
typedef std::map<std::string, asmjit::x86::Inst::Id> mnemMap;
typedef std::map<REG, asmjit::x86::Gp> x86RegMap;
typedef std::map<XREG, asmjit::x86::Reg> xmmRegMap;
typedef std::map<x86::Mnemonic, asmjit::x86::Inst::Id> x86MnemMap;

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

const x86RegMap lookupX86Registers = {
    {RAX, asmjit::x86::rax}, {RCX, asmjit::x86::rcx}, {RDX, asmjit::x86::rdx},
    {RBX, asmjit::x86::rbx}, {RSP, asmjit::x86::rsp}, {RBP, asmjit::x86::rbp},
    {RSI, asmjit::x86::rsi}, {RDI, asmjit::x86::rdi}, {R8, asmjit::x86::r8},
    {R9, asmjit::x86::r9},   {R10, asmjit::x86::r10}, {R11, asmjit::x86::r11},
    {R12, asmjit::x86::r12}, {R13, asmjit::x86::r13}, {R14, asmjit::x86::r14},
    {R15, asmjit::x86::r15},
};

const xmmRegMap lookupXmmRegisters = {{XMM0, asmjit::x86::xmm0}};

const x86MnemMap lookupX86Mnemonics = {
    {x86::MOV, asmjit::x86::Inst::kIdMov},
    {x86::ADD, asmjit::x86::Inst::kIdAdd},
    {x86::SUB, asmjit::x86::Inst::kIdSub},
    {x86::ENTER, asmjit::x86::Inst::kIdEnter},
    {x86::LEAVE, asmjit::x86::Inst::kIdLeave},
    {x86::RET, asmjit::x86::Inst::kIdRet},
    {x86::IMUL, asmjit::x86::Inst::kIdImul},
    {x86::IDIV, asmjit::x86::Inst::kIdIdiv},
    {x86::PUSH, asmjit::x86::Inst::kIdPush},
    {x86::POP, asmjit::x86::Inst::kIdPop},
    {x86::INC, asmjit::x86::Inst::kIdInc},
    {x86::MOVQ, asmjit::x86::Inst::kIdMovq},
    {x86::ADDSD, asmjit::x86::Inst::kIdAddsd},
    {x86::MULSD, asmjit::x86::Inst::kIdMulsd},
    {x86::SUBSD, asmjit::x86::Inst::kIdSubsd},
    {x86::CMP, asmjit::x86::Inst::kIdCmp},
    {x86::JMP, asmjit::x86::Inst::kIdJmp},
    {x86::JGE, asmjit::x86::Inst::kIdJge},
    {x86::JE, asmjit::x86::Inst::kIdJe},
    {x86::JLE, asmjit::x86::Inst::kIdJle},
    {x86::JNE, asmjit::x86::Inst::kIdJne},
    {x86::CALL, asmjit::x86::Inst::kIdCall}};

#endif  // ASM_HELPER_HPP