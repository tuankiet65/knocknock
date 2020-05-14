#include "cpu/instruction.h"

#include <fmt/format.h>
#include <glog/logging.h>

namespace cpu {

std::string Instruction::disassemble_opcode() const {
    switch (opcode_) {
        case Instruction::Opcode::RLC: return "RLC";
        case Instruction::Opcode::RRC: return "RRC";
        case Instruction::Opcode::RL: return "RL";
        case Instruction::Opcode::RR: return "RR";
        case Instruction::Opcode::SLA: return "SLA";
        case Instruction::Opcode::SRA: return "SRA";
        case Instruction::Opcode::SWAP: return "SWAP";
        case Instruction::Opcode::SRL: return "SRL";
        case Instruction::Opcode::BIT: return "BIT";
        case Instruction::Opcode::SET: return "SET";
        case Instruction::Opcode::RES: return "RES";
        case Instruction::Opcode::ADC: return "ADC";
        case Instruction::Opcode::ADD: return "ADD";
        case Instruction::Opcode::AND: return "AND";
        case Instruction::Opcode::CALL: return "CALL";
        case Instruction::Opcode::CCF: return "CCF";
        case Instruction::Opcode::CP: return "CP";
        case Instruction::Opcode::CPL: return "CPL";
        case Instruction::Opcode::DAA: return "DAA";
        case Instruction::Opcode::DEC: return "DEC";
        case Instruction::Opcode::DI: return "DI";
        case Instruction::Opcode::EI: return "EI";
        case Instruction::Opcode::HALT: return "HALT";
        case Instruction::Opcode::INC: return "INC";
        case Instruction::Opcode::JP: return "JP";
        case Instruction::Opcode::JR: return "JR";
        case Instruction::Opcode::LD: return "LD";
        case Instruction::Opcode::LDI: return "LDI";
        case Instruction::Opcode::LDD: return "LDD";
        case Instruction::Opcode::LDHL: return "LDHL";
        case Instruction::Opcode::NOP: return "NOP";
        case Instruction::Opcode::OR: return "OR";
        case Instruction::Opcode::POP: return "POP";
        case Instruction::Opcode::PUSH: return "PUSH";
        case Instruction::Opcode::RET: return "RET";
        case Instruction::Opcode::RETI: return "RETI";
        case Instruction::Opcode::RLA: return "RLA";
        case Instruction::Opcode::RLCA: return "RLCA";
        case Instruction::Opcode::RRA: return "RRA";
        case Instruction::Opcode::RRCA: return "RRCA";
        case Instruction::Opcode::SBC: return "SBC";
        case Instruction::Opcode::SCF: return "SCF";
        case Instruction::Opcode::STOP: return "STOP";
        case Instruction::Opcode::SUB: return "SUB";
        case Instruction::Opcode::XOR: return "XOR";
        case Instruction::Opcode::RST: return "RST";
    };

    DCHECK(false) << fmt::format("Unknown opcode: {}", opcode_);
    return fmt::format("[unknown opcode: {}]", opcode_);
}

std::string Instruction::disassemble_operand(
    Instruction::Operand operand) const {
    switch (operand) {
        case Instruction::Operand::None:
            // Not supposed to call this function on None
            DCHECK(false) << "Calling disassemble_operand on None operand";
            return "";
        case Instruction::Operand::PtrBC: return "(BC)";
        case Instruction::Operand::PtrC: return "(C)";
        case Instruction::Operand::PtrDE: return "(DE)";
        case Instruction::Operand::PtrHL: return "(HL)";
        case Instruction::Operand::A: return "A";
        case Instruction::Operand::AF: return "AF";
        case Instruction::Operand::B: return "B";
        case Instruction::Operand::BC: return "BC";
        case Instruction::Operand::C: return "C";
        case Instruction::Operand::D: return "D";
        case Instruction::Operand::DE: return "DE";
        case Instruction::Operand::E: return "E";
        case Instruction::Operand::H: return "H";
        case Instruction::Operand::HL: return "HL";
        case Instruction::Operand::L: return "L";
        case Instruction::Operand::SP: return "SP";
        case Instruction::Operand::FlagC: return "C";
        case Instruction::Operand::FlagNC: return "NC";
        case Instruction::Operand::FlagZ: return "Z";
        case Instruction::Operand::FlagNZ: return "NZ";

        // Special handling for immediate operands
        case Instruction::Operand::Imm8:
            return fmt::format("{:#02X}", imm8_.value());
        case Instruction::Operand::Imm8Sign:
            return fmt::format("{:#02d}", imm8sign_.value());
        case Instruction::Operand::PtrImm8:
            return fmt::format("({:#02X})", imm8_.value());
        case Instruction::Operand::Imm16:
            return fmt::format("{:#04X}", imm16_.value());
        case Instruction::Operand::PtrImm16:
            return fmt::format("({:#04X})", imm16_.value());
    }

    DCHECK(false) << fmt::format("Unknown operand: {}", operand);
    return fmt::format("[unknown operand: {}]", operand);
}  // namespace

Instruction::Instruction(Opcode opcode, Operand lhs, Operand rhs)
    : opcode_(opcode), lhs_(lhs), rhs_(rhs), imm8_(), imm8sign_(), imm16_() {
    // If rhs is not None, then lhs is also not None. Thus it's invalid that
    // rhs is not None but lhs is None.
    DCHECK(!((lhs_ == Operand::None) && (rhs_ != Operand::None)));
}

Instruction::Instruction(Opcode opcode, Operand lhs, Operand rhs, uint8_t imm8)
    : opcode_(opcode),
      lhs_(lhs),
      rhs_(rhs),
      imm8_(imm8),
      imm8sign_(),
      imm16_() {}

Instruction::Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         int8_t imm8sign)
    : opcode_(opcode),
      lhs_(lhs),
      rhs_(rhs),
      imm8_(),
      imm8sign_(imm8sign),
      imm16_() {}

Instruction::Instruction(Opcode opcode,
                         Operand lhs,
                         Operand rhs,
                         uint16_t imm16)
    : opcode_(opcode),
      lhs_(lhs),
      rhs_(rhs),
      imm8_(),
      imm8sign_(),
      imm16_(imm16) {}

std::string Instruction::disassemble() const {
    std::string result = disassemble_opcode();

    if (lhs_ != Instruction::Operand::None) {
        result += " " + disassemble_operand(lhs_);

        if (rhs_ != Instruction::Operand::None) {
            result += ", " + disassemble_operand(rhs_);
        }
    }

    return result;
}

}  // namespace cpu
