#include "cpu/micro_op_decoder.h"

#include <glog/logging.h>

#include <queue>

#include "cpu/instruction.h"
#include "cpu/micro_op.h"

#define OP cpu::Instruction::Opcode
#define ARG cpu::Instruction::Operand

namespace cpu {

namespace {

MicroOp::Operand convert_operand_8(ARG operand) {
    switch (operand) {
        case ARG::A: return MicroOp::Operand::A;
        case ARG::B: return MicroOp::Operand::B;
        case ARG::C: return MicroOp::Operand::C;
        case ARG::D: return MicroOp::Operand::D;
        case ARG::E: return MicroOp::Operand::E;
        case ARG::H: return MicroOp::Operand::H;
        case ARG::L: return MicroOp::Operand::L;
        case ARG::Imm8: return MicroOp::Operand::Imm8;
        default: DCHECK(false); return MicroOp::Operand::None;
    }
}

MicroOp::Operand convert_operand_flag(ARG operand) {
    switch (operand) {
        case ARG::FlagZ: return MicroOp::Operand::FlagZ;
        case ARG::FlagNZ: return MicroOp::Operand::FlagNZ;
        case ARG::FlagC: return MicroOp::Operand::FlagC;
        case ARG::FlagNC: return MicroOp::Operand::FlagNC;
        default: DCHECK(false); return MicroOp::Operand::None;
    }
}

void nop(std::queue<MicroOp> *uop_queue) {
    uop_queue->push(MicroOp(MicroOp::Opcode::NOP));
}

void jp(Instruction inst, std::queue<MicroOp> *uop_queue) {
    if (inst.lhs() == ARG::Imm16) {
        DCHECK(inst.rhs() == ARG::None);
        uop_queue->push(MicroOp(MicroOp::Opcode::JP, MicroOp::Operand::Imm16));
        return;
    }

    if (inst.lhs() == ARG::HL) {
        DCHECK(inst.rhs() == ARG::None);
        uop_queue->push(MicroOp(MicroOp::Opcode::JP, MicroOp::Operand::HL));
    }

    // JP Flag, Imm16
    auto flag = convert_operand_flag(inst.lhs());
    DCHECK(inst.rhs() == ARG::Imm16);
    uop_queue->push(MicroOp(MicroOp::Opcode::CSKIP, flag));
    uop_queue->push(MicroOp(MicroOp::Opcode::JP, MicroOp::Operand::Imm16));
}

void jr(Instruction inst, std::queue<MicroOp> *uop_queue) {
    // JR Imm8Sign
    if (inst.lhs() == ARG::Imm8Sign) {
        DCHECK(inst.rhs() == ARG::None);
        uop_queue->push(
            MicroOp(MicroOp::Opcode::JR, MicroOp::Operand::Imm8Sign));
        return;
    }

    // JR Flag, Imm8Sign
    auto flag = convert_operand_flag(inst.lhs());
    DCHECK(inst.rhs() == ARG::Imm8Sign);
    uop_queue->push(MicroOp(MicroOp::Opcode::CSKIP, flag));
    uop_queue->push(MicroOp(MicroOp::Opcode::JR, MicroOp::Operand::Imm8Sign));
}

void cp(Instruction inst, std::queue<MicroOp> *uop_queue) {
    DCHECK(inst.rhs() == ARG::None);

    MicroOp::Operand lhs;
    if (inst.lhs() == ARG::PtrHL) {
        uop_queue->push(MicroOp(MicroOp::Opcode::LD, MicroOp::Operand::Tmp8,
                                MicroOp::Operand::PtrHL));
        lhs = MicroOp::Operand::Tmp8;
    } else {
        lhs = convert_operand_8(inst.lhs());
    }

    uop_queue->push(MicroOp(MicroOp::Opcode::CP, lhs));
}

void swap(Instruction inst, std::queue<MicroOp> *uop_queue) {
    DCHECK(inst.rhs() == ARG::None);

    MicroOp::Operand lhs;
    if (inst.lhs() == ARG::PtrHL) {
        uop_queue->push(MicroOp(MicroOp::Opcode::LD, MicroOp::Operand::Tmp8,
                                MicroOp::Operand::PtrHL));
        lhs = MicroOp::Operand::Tmp8;
    } else {
        lhs = convert_operand_8(inst.lhs());
    }

    uop_queue->push(MicroOp(MicroOp::Opcode::SWAP, lhs));
}

void rlca(std::queue<MicroOp> *uop_queue) {
    uop_queue->push(MicroOp(MicroOp::Opcode::RLCA));
}

void rla(std::queue<MicroOp> *uop_queue) {
    uop_queue->push(MicroOp(MicroOp::Opcode::RLA));
}

void di(std::queue<MicroOp> *uop_queue) {
    uop_queue->push(MicroOp(MicroOp::Opcode::DI));
}

}  // namespace

// static
void MicroOpDecoder::decode(Instruction inst, std::queue<MicroOp> *uop_queue) {
    switch (inst.opcode()) {
        case OP::NOP: nop(uop_queue); break;
        case OP::JP: jp(inst, uop_queue); break;
        case OP::JR: jr(inst, uop_queue); break;
        case OP::CP: cp(inst, uop_queue); break;
        case OP::SWAP: swap(inst, uop_queue); break;
        case OP::RLCA: rlca(uop_queue); break;
        case OP::RLA: rla(uop_queue); break;
        case OP::DI: di(uop_queue); break;
        default:
            DCHECK(false) << "No rules to decode this instruction into uop: "
                          << inst.disassemble();
    }
}

}  // namespace cpu
