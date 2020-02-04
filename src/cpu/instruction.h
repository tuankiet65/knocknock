#pragma once

#include <optional>
#include <string>

#include "memory/memory.h"

namespace cpu {

/**
 * Class representing an Instruction.
 *
 * An Instruction is an instruction which can be understood by the CPU. An
 * Instruction contains an opcode, which represents the operation to be done,
 * and either zero, one or two operands, which are arguments to the operation.
 */
class Instruction {
public:
    /**
     * Enum of all available opcodes.
     */
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

    /**
     * Enum of all possible operands.
     */
    enum class Operand {
        None, /**< Filler when an operand is not needed */

        // 8 bit registers and its pointers.
        A,
        B,
        C,
        PtrC, /** (C), pointer to ($FF00 + C) */
        D,
        E,
        H,
        L,

        // 16 bit registers and its pointers.
        AF,
        BC,
        PtrBC, /**< (BC) */
        DE,
        PtrDE, /**< (DE) */
        HL,
        PtrHL,      /**< (HL) */
        PtrHLPlus,  /**< (HL+) */
        PtrHLMinus, /**< (HL-) */
        SP,

        // Individual flag in the flag register
        FlagC,  /**<  Carry */
        FlagNC, /**< ~Carry */
        FlagZ,  /**<  Zero  */
        FlagNZ, /**< ~Zero  */

        // Immediates and its registers
        Imm8,     /**< unsigned 8-bit immediate */
        PtrImm8,  /**< (Imm8), pointer to ($FF00 + Imm8) */
        Imm8Sign, /**< signed 8-bit immediate (for relative jumps) */
        Imm16,    /**< unsigned 16-bit immediate */
        PtrImm16, /**< (Imm16) */
    };

    /**
     * Construct an Instruction with the specified opcode and operands, and
     * without immediates.
     *
     * @param opcode Opcode of the instruction.
     * @param lhs the operand on the left hand side, or None if not available.
     * @param rhs the operand on the right hand side, or None if not available.
     */
    Instruction(Opcode opcode, Operand lhs, Operand rhs);

    /**
     * Construct an Instruction with the specified opcode and operands, and
     * with an unsigned 8-bit immediate. This construction is marked as explicit
     * to prevent implicit type conversion between int types.
     *
     * @param opcode Opcode of the instruction.
     * @param lhs the operand on the left hand side, or None if not available.
     * @param rhs the operand on the right hand side, or None if not available.
     * @param imm8 the unsigned 8-bit immediate.
     */
    explicit Instruction(Opcode opcode, Operand lhs, Operand rhs, uint8_t imm8);

    /**
     * Construct an Instruction with the specified opcode and operands, and
     * with a signed 8-bit immediate. This construction is marked as explicit
     * to prevent implicit type conversion between int types.
     *
     * @param opcode Opcode of the instruction.
     * @param lhs the operand on the left hand side, or None if not available.
     * @param rhs the operand on the right hand side, or None if not available.
     * @param imm8sign the signed 8-bit immediate.
     */
    explicit Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         int8_t imm8sign);

    /**
     * Construct an Instruction with the specified opcode and operands, and
     * with an unsigned 16-bit immediate. This construction is marked as
     * explicit to prevent implicit type conversion between int types.
     *
     * @param opcode Opcode of the instruction.
     * @param lhs the operand on the left hand side, or None if not available.
     * @param rhs the operand on the right hand side, or None if not available.
     * @param imm16 the unsigned 16-bit immediate.
     */
    explicit Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         uint16_t imm16);

    /**
     * Get the opcode.
     * @return Opcode the opcode.
     */
    Opcode opcode() const { return opcode_; }

    /**
     * Get the left hand sign operand.
     * @return Operand the left hand sign operand.
     */
    Operand lhs() const { return lhs_; }

    /**
     * Get the right hand sign operand.
     * @return Operand the right hand sign operand.
     */
    Operand rhs() const { return rhs_; }

    /**
     * Get the unsigned 8-bit immediate, if available.
     * @return std::optional<uint8_t> the immediate wrapped around
     * std::optional. The user must check whether a value is contained or not.
     */
    std::optional<uint8_t> imm8() const { return imm8_; }

    /**
     * Get the signed 8-bit immediate, if available.
     * @return std::optional<int8_t> the immediate wrapped around
     * std::optional. The user must check whether a value is contained or not.
     */
    std::optional<int8_t> imm8sign() const { return imm8sign_; }

    /**
     * Get the unsigned 16-bit immediate, if available.
     * @return std::optional<uint16_t> the immediate wrapped around
     * std::optional. The user must check whether a value is contained or not.
     */
    std::optional<uint16_t> imm16() const { return imm16_; }

    /**
     * Return the assembly command that represents this instruction.
     * @return std::string the assembly command for this instruction.
     */
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
