#pragma once

#include <optional>
#include <string>

#include "memory/memory.h"

namespace cpu {

class Instruction {
public:
    enum class Opcode { RLC, RRC, RL, RR, SLA, SRA, SLL, SRL, BIT, RES, SET };
    enum class Operand {
        None,  // for when an opcode accepts 0 or 1 operands
        RegA,
        RegB,
        RegC,
        RegD,
        RegE,
        RegH,
        RegL,
        MemRegHL  // (HL)
    };

    Instruction(Opcode opcode, Operand lhs, Operand rhs);

    static std::optional<Instruction> decode(const memory::Memory *mem,
                                             memory::MemoryAddr *addr);

    Opcode opcode() const { return opcode_; }
    Operand lhs() const { return lhs_; }
    Operand rhs() const { return rhs_; }

    std::string disassemble() const;

private:
    Opcode opcode_;
    Operand lhs_, rhs_;
};

}  // namespace cpu
