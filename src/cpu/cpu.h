#pragma once

#include <queue>

#include "cpu/decoder.h"
#include "cpu/micro_op.h"
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

    Register8 imm8_, tmp8_;
    Register8Sign imm8sign_;
    Register16 imm16_, tmp16_;

    memory::Memory *mem_;
    memory::MemoryAddr pc_;

    Decoder decoder_;

    std::queue<MicroOp> uop_queue_;

    bool skip_next_instruction_;

    Register8 *operand8_to_register(MicroOp::Operand operand);
    void execute_uop();

    void nop();
    void jp(MicroOp::Operand lhs);
    void jr(MicroOp::Operand lhs);
    void cskip(MicroOp::Operand lhs);
    void ld(MicroOp::Operand lhs, MicroOp::Operand rhs);
    void cp(MicroOp::Operand lhs);
    void swap(MicroOp::Operand lhs);
};

}  // namespace cpu
