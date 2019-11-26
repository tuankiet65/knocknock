#include "cpu/cpu.h"

#include <glog/logging.h>

#include "memory/memory.h"

namespace cpu {

CPU::CPU(memory::Memory *memory)
    : mem_(memory),
      a_("A"),
      f_("F"),
      b_("B"),
      c_("C"),
      d_("D"),
      e_("E"),
      h_("H"),
      l_("L"),
      af_(&a_, &b_),
      bc_(&b_, &c_),
      de_(&d_, &e_),
      hl_(&h_, &l_),
      sp_("SP"),
      pc_("PC"),
      cycles_remaining_(0) {
    DCHECK(mem_);

    // initialize all registers
    af_.write(0x01b0);
    bc_.write(0x0013);
    de_.write(0x00d8);
    hl_.write(0x014d);
    sp_.write(0xfffe);
    pc_.write(0x0100);

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

void CPU::step() {
    if (cycles_remaining_) {
        cycles_remaining_--;
        return;
    }

    Instruction inst = CPU::decode();
    LOG(INFO) << "Executing instruction: " << inst.disassembly();
    inst.execute();
    cycles_remaining_ = inst.cycles();
}

}  // namespace cpu
