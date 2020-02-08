#include "cpu/cpu.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <iostream>

#include "cpu/micro_op.h"
#include "cpu/micro_op_decoder.h"
#include "memory/memory.h"

namespace cpu {

CPU::CPU(memory::Memory *memory)
    : a_(),
      b_(),
      c_(),
      d_(),
      e_(),
      h_(),
      l_(),
      f_(),
      af_(&a_, &b_),
      bc_(&b_, &c_),
      de_(&d_, &e_),
      hl_(&h_, &l_),
      sp_(),
      imm8_(),
      tmp8_(),
      imm8sign_(),
      imm16_(),
      tmp16_(),
      mem_(memory),
      decoder_(mem_, &pc_) {
    DCHECK(mem_);

    // initialize all registers
    af_.write(0x01b0);
    bc_.write(0x0013);
    de_.write(0x00d8);
    hl_.write(0x014d);
    sp_.write(0xfffe);
    pc_ = 0x0100;

    // initialize IO register
    mem_->write(0xff05, 0x00);  //  TIMA
    mem_->write(0xff06, 0x00);  //  TMA
    mem_->write(0xff07, 0x00);  //  TAC
    mem_->write(0xff10, 0x80);  //  NR10
    mem_->write(0xff11, 0xbf);  //  NR11
    mem_->write(0xff12, 0xf3);  //  NR12
    mem_->write(0xff14, 0xbf);  //  NR14
    mem_->write(0xff16, 0x3f);  //  NR21
    mem_->write(0xff17, 0x00);  //  NR22
    mem_->write(0xff19, 0xbf);  //  NR24
    mem_->write(0xff1a, 0x7f);  //  NR30
    mem_->write(0xff1b, 0xff);  //  NR31
    mem_->write(0xff1c, 0x9f);  //  NR32
    mem_->write(0xff1e, 0xbf);  //  NR33
    mem_->write(0xff20, 0xff);  //  NR41
    mem_->write(0xff21, 0x00);  //  NR42
    mem_->write(0xff22, 0x00);  //  NR43
    mem_->write(0xff23, 0xbf);  //  NR30
    mem_->write(0xff24, 0x77);  //  NR50
    mem_->write(0xff25, 0xf3);  //  NR51
    mem_->write(0xff26, 0xf1);  //  NR52
    mem_->write(0xff40, 0x91);  //  LCDC
    mem_->write(0xff42, 0x00);  //  SCY
    mem_->write(0xff43, 0x00);  //  SCX
    mem_->write(0xff45, 0x00);  //  LYC
    mem_->write(0xff47, 0xfc);  //  BGP
    mem_->write(0xff48, 0xff);  //  OBP0
    mem_->write(0xff49, 0xff);  //  OBP1
    mem_->write(0xff4a, 0x00);  //  WY
    mem_->write(0xff4b, 0x00);  //  WX
    mem_->write(0xffff, 0x00);  //  IE
}

Register8 *CPU::operand8_to_register(MicroOp::Operand operand) {
    switch (operand) {
        case MicroOp::Operand::A: return &a_;
        case MicroOp::Operand::B: return &b_;
        case MicroOp::Operand::C: return &c_;
        case MicroOp::Operand::D: return &d_;
        case MicroOp::Operand::E: return &e_;
        case MicroOp::Operand::H: return &h_;
        case MicroOp::Operand::L: return &l_;
        case MicroOp::Operand::Tmp8: return &tmp8_;
        case MicroOp::Operand::Imm8: return &imm8_;
        default:
            CHECK(false) << fmt::format("Wrong operand to request: {}",
                                        operand);
            // Will never reach, but GCC will complains if I miss this.
            return &a_;
    }
}

bool CPU::step() {
    // TODO remove this
    if (pc_ < 0x100) {
        return false;
    }

    if (!uop_queue_.empty()) {
        execute_uop();
        return true;
    }

    decoder_.step();

    if (!decoder_.decoded_instruction().has_value()) {
        return true;
    }

    Instruction inst = decoder_.decoded_instruction().value();
    if (inst.imm8().has_value()) {
        imm8_.write(inst.imm8().value());
    }
    if (inst.imm8sign().has_value()) {
        imm8sign_.write(inst.imm8sign().value());
    }
    if (inst.imm16().has_value()) {
        imm16_.write(inst.imm16().value());
    }

    LOG(ERROR) << fmt::format("{:#05x} {}", pc_, inst.disassemble());

    if (skip_next_instruction_) {
        skip_next_instruction_ = false;
        LOG(ERROR) << "Skipping this instruction";
    } else {
        MicroOpDecoder::decode(inst, &uop_queue_);
    }

    return true;
}

void CPU::execute_uop() {
    MicroOp uop = uop_queue_.front();
    uop_queue_.pop();

    switch (uop.opcode()) {
        case MicroOp::Opcode::NOP: nop(); break;
        case MicroOp::Opcode::JP: jp(uop.lhs()); break;
        case MicroOp::Opcode::LD: ld(uop.lhs(), uop.rhs()); break;
        case MicroOp::Opcode::CP: cp(uop.lhs()); break;
        case MicroOp::Opcode::CSKIP: cskip(uop.lhs()); break;
        case MicroOp::Opcode::JR: jr(uop.lhs()); break;
        default:
            DCHECK(false) << "Unknown uop to execute: " << uop.disassemble();
            break;
    }
}

void CPU::nop() {}

void CPU::jp(MicroOp::Operand lhs) {
    switch (lhs) {
        case MicroOp::Operand::Imm16: pc_ = imm16_.read(); break;
        case MicroOp::Operand::HL: pc_ = hl_.read(); break;
        default: DCHECK(false); break;
    }
}

void CPU::jr(MicroOp::Operand lhs) {
    DCHECK(lhs == MicroOp::Operand::Imm8Sign);
    pc_ += imm8sign_.read();
}

void CPU::cskip(MicroOp::Operand lhs) {
    switch (lhs) {
        case MicroOp::Operand::FlagC:
            skip_next_instruction_ = f_.get(FlagRegister::Flag::Carry);
            break;
        case MicroOp::Operand::FlagNC:
            skip_next_instruction_ = !f_.get(FlagRegister::Flag::Carry);
            break;
        case MicroOp::Operand::FlagZ:
            skip_next_instruction_ = f_.get(FlagRegister::Flag::Zero);
            break;
        case MicroOp::Operand::FlagNZ:
            skip_next_instruction_ = !f_.get(FlagRegister::Flag::Zero);
            break;
        default: DCHECK(false); break;
    }
}

void CPU::ld(MicroOp::Operand lhs, MicroOp::Operand rhs) {
    if (lhs == MicroOp::Operand::Tmp8 && rhs == MicroOp::Operand::PtrHL) {
        tmp8_.write(mem_->read(hl_.read()));
    }

    DCHECK(false) << "Unknown ld to execute";
}

void CPU::cp(MicroOp::Operand lhs) {
    LOG(ERROR) << "Half carry flag not implemented!";

    Register8 *reg = operand8_to_register(lhs);

    f_.set(FlagRegister::Flag::Subtract);

    if (a_.read() == reg->read()) {
        f_.set(FlagRegister::Flag::Zero);
    } else {
        f_.clear(FlagRegister::Flag::Zero);
    }

    if (a_.read() < reg->read()) {
        f_.set(FlagRegister::Flag::Carry);
    } else {
        f_.clear(FlagRegister::Flag::Carry);
    }
}

}  // namespace cpu
