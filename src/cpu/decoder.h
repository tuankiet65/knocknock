#pragma once

#include <optional>

#include "cpu/instruction.h"
#include "memory/memory.h"

namespace cpu {

class Decoder {
public:
    Decoder(const memory::Memory *memory, memory::MemoryAddr *pc);

    void step();

    std::optional<Instruction> decoded_instruction() {
        return decoded_instruction_;
    }

private:
    enum class State {
        INITIAL,
        OPCODE,
        CB_PREFIX,
        IMMEDIATE_8,
        IMMEDIATE_8_SIGN,
        IMMEDIATE_16_LOW,
        IMMEDIATE_16_HIGH
    };

    void reset();

    void decode_cb(uint8_t opcode);
    void decode_ld_8bit(uint8_t opcode);
    void decode_alu(uint8_t opcode);
    void decode_assorted(uint8_t opcode);

    bool needs_imm8(Instruction::Operand operand);
    bool needs_imm8sign(Instruction::Operand operand);
    bool needs_imm16(Instruction::Operand operand);

    State state_;

    const memory::Memory *memory_;
    memory::MemoryAddr *pc_;

    Instruction::Opcode opcode_;
    Instruction::Operand lhs_, rhs_;
    std::optional<uint8_t> imm8_;
    std::optional<int8_t> imm8sign_;
    std::optional<uint16_t> imm16_;

    std::optional<Instruction> decoded_instruction_;
};

}  // namespace cpu
