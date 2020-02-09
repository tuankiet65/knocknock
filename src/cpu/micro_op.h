#pragma once

#include <queue>
#include <string>

#include "cpu/instruction.h"

namespace cpu {

class MicroOp {
public:
    enum class Opcode { NOP, JP, CP, LD, CSKIP, JR, SWAP, RLCA};
    enum class Operand {
        None,
        Imm16,
        Imm8,
        Imm8Sign,
        A,
        B,
        C,
        D,
        E,
        H,
        L,
        Tmp8,
        Tmp16,
        PtrHL,
        HL,

        // Flags
        FlagZ,
        FlagNZ,
        FlagC,
        FlagNC
    };

    MicroOp(Opcode opcode,
            Operand lhs = Operand::None,
            Operand rhs = Operand::None)
        : opcode_(opcode), lhs_(lhs), rhs_(rhs) {}

    Opcode opcode() const { return opcode_; }
    Operand lhs() const { return lhs_; }
    Operand rhs() const { return rhs_; }

    std::string disassemble() const;

private:
    Opcode opcode_;
    Operand lhs_, rhs_;
};

}  // namespace cpu
