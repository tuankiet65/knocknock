#pragma once

#include <queue>

#include "cpu/decoder.h"
#include "cpu/operands.h"
#include "memory/memory.h"

namespace cpu {

class CPU {
public:
    CPU(memory::Memory *mem);

    bool step();

private:
    Register8 a_, b_, c_, d_, e_, h_, l_;
    FlagRegister f_;
    Register16Mirror af_, bc_, de_, hl_;
    Register16 sp_;

    Register8 imm8_;
    Register8Sign imm8sign_;
    Register16 imm16_;

    memory::Memory *mem_;
    memory::MemoryAddr pc_;

    Decoder decoder_;

    bool interrupt_enabled_;

    std::optional<Operand8*> get_operand8(Instruction::Operand lhs);
    std::optional<Operand16*> get_operand16(Instruction::Operand lhs);

    void execute_instruction(Instruction inst);

    // TODO: remove these
    Memory8 ptr_bc_, ptr_de_, ptr_hl_, ptr_imm16_;
    Memory8HiMem ptr_c_, ptr_imm8_;

    void nop();
    void jp(Instruction::Operand lhs, Instruction::Operand rhs);
    void jr(Instruction::Operand lhs, Instruction::Operand rhs);
    void ld(Instruction::Operand lhs, Instruction::Operand rhs);
    void cp(Instruction::Operand lhs);
    void swap(Instruction::Operand lhs);
    void rlca();
    void rla();
    void di();
    void ei();
    void call(Instruction::Operand lhs, Instruction::Operand rhs);
    void ret();
    void push(Instruction::Operand lhs);
    void pop(Instruction::Operand lhs);
    void inc(Instruction::Operand lhs);
    void ldi(Instruction::Operand lhs, Instruction::Operand rhs);
    // OR instruction, named "or_" to avoid conflict with the "or" keyword.
    void or_(Instruction::Operand lhs);
    // AND instruction, named "and_" to avoid conflict with the "and" keyword.
    void and_(Instruction::Operand lhs);
    void dec(Instruction::Operand lhs);
    // XOR instruction, named "xor_" to avoid conflict with the "xor" keyword.
    void xor_(Instruction::Operand lhs);
    void add(Instruction::Operand lhs, Instruction::Operand rhs);
    void ldd(Instruction::Operand lhs, Instruction::Operand rhs);
    void sub(Instruction::Operand lhs);
    void srl(Instruction::Operand lhs);
    void rr(Instruction::Operand lhs);
    void adc(Instruction::Operand lhs);

    void push_to_stack(uint16_t value);
    uint16_t pop_from_stack();
};

}  // namespace cpu
