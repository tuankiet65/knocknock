#include "cpu/micro_op.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <queue>
#include <string>

#include "cpu/instruction.h"

#define OP cpu::Instruction::Opcode
#define ARG cpu::Instruction::Operand

namespace cpu {

namespace {

std::string disassemble_opcode(MicroOp::Opcode opcode) {
    switch (opcode) {
        case MicroOp::Opcode::CP: return "CP";
        case MicroOp::Opcode::JP: return "JP";
        case MicroOp::Opcode::LD: return "LD";
        case MicroOp::Opcode::NOP: return "NOP";
        case MicroOp::Opcode::CSKIP: return "CSKIP";
        case MicroOp::Opcode::JR: return "JR";
        case MicroOp::Opcode::SWAP: return "SWAP";
        case MicroOp::Opcode::RLCA: return "RLCA";
        case MicroOp::Opcode::RLA: return "RLA";
        case MicroOp::Opcode::DI: return "DI";
    }

    DCHECK(false) << fmt::format("Unknown uop opcode: {}", opcode);
    return fmt::format("[unknown opcode: {}]", opcode);
}

std::string disassemble_operand(MicroOp::Operand operand) {
    switch (operand) {
        case MicroOp::Operand::A: return "A";
        case MicroOp::Operand::B: return "B";
        case MicroOp::Operand::C: return "C";
        case MicroOp::Operand::D: return "D";
        case MicroOp::Operand::E: return "E";
        case MicroOp::Operand::H: return "H";
        case MicroOp::Operand::L: return "L";
        case MicroOp::Operand::Imm8: return "Imm8";
        case MicroOp::Operand::Imm16: return "Imm16";
        case MicroOp::Operand::Tmp8: return "Tmp8";
        case MicroOp::Operand::Tmp16: return "Tmp16";
        case MicroOp::Operand::PtrHL: return "(HL)";
        default:
            DCHECK(false) << fmt::format("Unknown uop operand: {}", operand);
            return fmt::format("[unknown operand: {}]", operand);
    }
}

}  // namespace

std::string MicroOp::disassemble() const {
    std::string result = disassemble_opcode(opcode_);

    if (lhs_ != Operand::None) {
        result += " " + disassemble_operand(lhs_);
        if (rhs_ != Operand::None) {
            result += ", " + disassemble_operand(rhs_);
        }
    }

    return result;
}

}  // namespace cpu
