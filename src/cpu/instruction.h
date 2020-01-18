#pragma once

#include <optional>
#include <string>

#include "memory/memory.h"

namespace cpu {

class Instruction {
public:
    enum class Opcode {
        NOP,
        RLC,
        RRC,
        RL,
        RR,
        SLA,
        SRA,
        SLL,
        SRL,
        BIT,
        RES,
        SET,
        ADC,
        ADD,
        AND,
        CALL,
        CCF,
        CP,
        CPL,
        DAA,
        DEC,
        DI,
        EI,
        HALT,
        INC,
        JP,
        JR,
        LD,
        LDH,
        LDHL,
        OR,
        POP,
        PUSH,
        RET,
        RETI,
        RLA,
        RLCA,
        RRA,
        RRCA,
        SBC,
        SCF,
        STOP,
        SUB,
        XOR,
        RST
    };

    enum class Operand {
        None,  // for when an opcode accepts 0 or 1 operands

        // 8 bit registers
        A,
        B,
        C,
        PtrC,  // (C), (FF00 + C)
        D,
        E,
        H,
        L,

        // 16 bit registers and its pointers
        AF,
        BC,
        PtrBC,  // (BC)
        DE,
        PtrDE,  // (DE)
        HL,
        PtrHL,       // (HL)
        PtrHLPlus,   // (HL+)
        PtrHLMinus,  // (HL-)
        SP,

        // Individual flag in the flag register
        FlagC,   // Carry
        FlagNC,  // ~Carry
        FlagZ,   // Zero
        FlagNZ,  // ~Zero

        // Immediates
        Imm8,      // uint8_t
        PtrImm8,   // (Imm8) = (FF00 + Imm8)
        Imm8Sign,  // int8_t
        Imm16,     // uint16_t
        PtrImm16,  // (Imm16)
    };

    // Instruction(Opcode opcode);
    // Instruction(Opcode opcode, Operand lhs);
    Instruction(Opcode opcode, Operand lhs, Operand rhs);

    // Mark these constructors as explicit to prevent type conversion between
    // uint8_t, int8_t and uint16_t.
    explicit Instruction(Opcode opcode, Operand lhs, Operand rhs, uint8_t imm8);
    explicit Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         int8_t imm8sign);
    explicit Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         uint16_t imm16);

    // static std::optional<Instruction> decode(const memory::Memory *mem,
    //                                          memory::MemoryAddr *addr);

    Opcode opcode() const { return opcode_; }
    Operand lhs() const { return lhs_; }
    Operand rhs() const { return rhs_; }
    std::optional<uint8_t> imm8() const { return imm8_; }
    std::optional<int8_t> imm8sign() const { return imm8sign_; }
    std::optional<uint16_t> imm16() const { return imm16_; }

    std::string disassemble() const;

private:
    std::string disassemble_opcode() const;
    std::string disassemble_operand(Operand operand) const;

    Opcode opcode_;
    Operand lhs_, rhs_;

    std::optional<uint8_t> imm8_;
    std::optional<int8_t> imm8sign_;
    std::optional<uint16_t> imm16_;
};

}  // namespace cpu
