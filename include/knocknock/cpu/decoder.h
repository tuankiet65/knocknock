#pragma once

#include <optional>

#include "knocknock/cpu/instruction.h"
#include "knocknock/memory/memory.h"

namespace cpu {

class Decoder {
public:
    Decoder(const memory::Memory *memory, memory::MemoryAddr *pc);

    void step();

    [[nodiscard]] std::optional<Instruction> decoded_instruction() const {
        return decoded_instruction_;
    }

private:
    enum class State {
        OPCODE,
        CB_PREFIX,
        IMMEDIATE_8,
        IMMEDIATE_8_SIGN,
        IMMEDIATE_16_LOW,
        IMMEDIATE_16_HIGH
    };

    void reset();
    void assemble();

    bool decode_cb(uint8_t opcode);
    bool decode_ld_8bit(uint8_t opcode);
    bool decode_alu(uint8_t opcode);
    bool decode_rst(uint8_t opcode);
    bool decode_assorted(uint8_t opcode);

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
