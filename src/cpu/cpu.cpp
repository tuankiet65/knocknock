#include "cpu/cpu.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <iostream>

#include "memory/memory.h"

namespace cpu {

CPU::CPU(memory::Memory *memory)
    : mem_(memory),
      a_(),
      b_(),
      c_(),
      d_(),
      e_(),
      h_(),
      l_(),
      f_(),
      sp_(),
      af_(&a_, &b_),
      bc_(&b_, &c_),
      de_(&d_, &e_),
      hl_(&h_, &l_),
      imm8_(),
      tmp8_(),
      imm8sign_(),
      imm16_(),
      tmp16_(),
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

bool CPU::step() {
    // TODO remove this
    if (pc_ < 0x100) {
        return false;
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

    return true;
}

}  // namespace cpu
