#pragma once

#include <queue>

#include "knocknock/cpu/decoder.h"
#include "knocknock/cpu/operands.h"
#include "knocknock/memory/memory.h"
#include "knocknock/peripherals/tickable.h"

namespace cpu {

class CPU : public peripherals::Tickable {
public:
    CPU(memory::Memory *mem);

    // clock::Tickable::
    void tick() override;

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

    std::optional<Operand8 *> get_operand8(Instruction::Operand lhs);
    std::optional<Operand16 *> get_operand16(Instruction::Operand lhs);

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
    void ret(Instruction::Operand lhs);
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
    void adc(Instruction::Operand lhs, Instruction::Operand rhs);
    void sbc(Instruction::Operand lhs, Instruction::Operand rhs);
    void cpl();
    void scf();
    void ccf();
    void rra();
    void rlc(Instruction::Operand lhs);
    void rl(Instruction::Operand lhs);
    void rrc(Instruction::Operand lhs);
    void rrca();
    void sla(Instruction::Operand lhs);
    void sra(Instruction::Operand lhs);
    void rst(Instruction::Operand lhs);
    void bit(Instruction::Operand lhs, Instruction::Operand rhs);
    void res(Instruction::Operand lhs, Instruction::Operand rhs);
    void set(Instruction::Operand lhs, Instruction::Operand rhs);
    void daa();
    void ldhl(Instruction::Operand lhs, Instruction::Operand rhs);

    void push_to_stack(uint16_t value);
    uint16_t pop_from_stack();
};

}  // namespace cpu
