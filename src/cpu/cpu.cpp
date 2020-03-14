#include "cpu/cpu.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <iostream>

#include "memory/memory.h"

namespace cpu {

namespace {

uint8_t low_nibble(uint8_t value) {
    return value & 0x0Fu;
}

uint8_t high_nibble(uint8_t value) {
    return value >> 4u;
}

}  // namespace

using Opcode = Instruction::Opcode;
using Operand = Instruction::Operand;

CPU::CPU(memory::Memory *memory)
    : a_(),
      b_(),
      c_(),
      d_(),
      e_(),
      h_(),
      l_(),
      f_(),
      af_(&a_, &f_),
      bc_(&b_, &c_),
      de_(&d_, &e_),
      hl_(&h_, &l_),
      sp_(),
      imm8_(),
      imm8sign_(),
      imm16_(),
      mem_(memory),
      decoder_(mem_, &pc_),
      interrupt_enabled_(false),
      ptr_bc_(mem_, bc_),
      ptr_de_(mem_, de_),
      ptr_hl_(mem_, hl_),
      ptr_imm16_(mem_, imm16_),
      ptr_c_(mem_, c_),
      ptr_imm8_(mem_, imm8_) {
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

std::optional<Operand8 *> CPU::get_operand8(Operand operand) {
    switch (operand) {
        case Operand::A: return &a_;
        case Operand::B: return &b_;
        case Operand::C: return &c_;
        case Operand::PtrC: return &ptr_c_;
        case Operand::D: return &d_;
        case Operand::E: return &e_;
        case Operand::H: return &h_;
        case Operand::L: return &l_;
        case Operand::Imm8: return &imm8_;
        case Operand::PtrImm8: return &ptr_imm8_;
        case Operand::PtrBC: return &ptr_bc_;
        case Operand::PtrDE: return &ptr_de_;
        case Operand::PtrHL: return &ptr_hl_;
        case Operand::PtrImm16: return &ptr_imm16_;
    }

    return {};
}

std::optional<Operand16 *> CPU::get_operand16(Operand operand) {
    switch (operand) {
        case Operand::AF: return &af_;
        case Operand::BC: return &bc_;
        case Operand::DE: return &de_;
        case Operand::HL: return &hl_;
        case Operand::SP: return &sp_;
        case Operand::Imm16: return &imm16_;
    }

    return {};
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

    execute_instruction(inst);

    return true;
}

void CPU::execute_instruction(Instruction inst) {
    switch (inst.opcode()) {
        case Opcode::NOP: nop(); break;
        case Opcode::JP: jp(inst.lhs(), inst.rhs()); break;
        case Opcode::LD: ld(inst.lhs(), inst.rhs()); break;
        case Opcode::CP: cp(inst.lhs()); break;
        case Opcode::JR: jr(inst.lhs(), inst.rhs()); break;
        case Opcode::SWAP: swap(inst.lhs()); break;
        case Opcode::RLCA: rlca(); break;
        case Opcode::RLA: rla(); break;
        case Opcode::DI: di(); break;
        case Opcode::EI: ei(); break;
        case Opcode::CALL: call(inst.lhs(), inst.rhs()); break;
        case Opcode::RET: ret(); break;
        case Opcode::PUSH: push(inst.lhs()); break;
        case Opcode::POP: pop(inst.lhs()); break;
        case Opcode::INC: inc(inst.lhs()); break;
        case Opcode::LDI: ldi(inst.lhs(), inst.rhs()); break;
        case Opcode::OR: or_(inst.lhs()); break;
        case Opcode::AND: and_(inst.lhs()); break;
        case Opcode::DEC: dec(inst.lhs()); break;
        case Opcode::XOR: xor_(inst.lhs()); break;
        case Opcode::ADD: add(inst.lhs(), inst.rhs()); break;
        case Opcode::LDD: ldd(inst.lhs(), inst.rhs()); break;
        case Opcode::SUB: sub(inst.lhs()); break;
        case Opcode::SRL: srl(inst.lhs()); break;
        case Opcode::RR: rr(inst.lhs()); break;
        // TODO: RRA does not set the zero flag.
        case Opcode::RRA: rr(Operand::A); break;

        default:
            DCHECK(false) << "Instruction not recognized: "
                          << inst.disassemble();
            break;
    }
}

void CPU::nop() {}

// Status: NOT cycle accurate
void CPU::jp(Operand lhs, Operand rhs) {
    // One operand
    switch (lhs) {
        case Operand::Imm16: pc_ = imm16_.read(); return;
        case Operand::HL: pc_ = hl_.read(); return;
    }

    // Two operands
    bool should_jump = false;
    switch (lhs) {
        case Operand::FlagC: should_jump = f_.carry; break;
        case Operand::FlagNC: should_jump = !f_.carry; break;
        case Operand::FlagZ: should_jump = f_.zero; break;
        case Operand::FlagNZ: should_jump = !f_.zero; break;
    }

    if (should_jump) {
        DCHECK(rhs != Operand::Imm16);
        pc_ = imm16_.read();
    }
}

// Status: NOT cycle accurate
void CPU::jr(Operand lhs, Operand rhs) {
    // One operand
    if (lhs == Operand::Imm8Sign) {
        pc_ += imm8sign_.read();
        return;
    }

    // Two operands
    bool should_jump = false;
    switch (lhs) {
        case Operand::FlagC: should_jump = f_.carry; break;
        case Operand::FlagNC: should_jump = !f_.carry; break;
        case Operand::FlagZ: should_jump = f_.zero; break;
        case Operand::FlagNZ: should_jump = !f_.zero; break;
    }

    if (should_jump) {
        DCHECK(rhs != Operand::Imm8Sign);
        pc_ += imm8sign_.read();
    }
}

void CPU::ld(Operand lhs, Operand rhs) {
    // lhs and rhs are Operand8s
    auto l8 = get_operand8(lhs), r8 = get_operand8(rhs);
    if (l8 && r8) {
        (*l8)->write((*r8)->read());
        return;
    }

    // lhs and rhs are Operand16s
    auto l16 = get_operand16(lhs), r16 = get_operand16(rhs);
    if (l16 && r16) {
        (*l16)->write((*r16)->read());
        return;
    }

    DCHECK(false);
}

void CPU::cp(Operand lhs) {
    auto reg = get_operand8(lhs);
    DCHECK(reg);

    uint8_t x = a_.read(), y = (*reg)->read();

    f_.carry = (x < y);
    f_.half_carry = (low_nibble(x) < high_nibble(y));
    f_.subtract = true;
    f_.zero = (x == y);
}

void CPU::swap(Operand lhs) {
    std::optional<Operand8 *> reg = get_operand8(lhs);
    DCHECK(!reg.has_value());

    uint8_t val = (*reg)->read();
    uint8_t new_val = (val >> 4u) |               // top nibble
                      (val & 0b00001111u) << 4u;  // bottom nibble

    (*reg)->write(new_val);

    f_.zero = (new_val == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = false;
}

void CPU::rlca() {
    // Carry contains the 7th bit
    f_.carry = (a_.read() >> 7u);

    uint8_t new_val = a_.read();
    new_val = (new_val << 1u) | (new_val >> 7u);
    a_.write(new_val);

    f_.zero = false;
    f_.subtract = false;
    f_.half_carry = false;
}

void CPU::rla() {
    uint8_t val = a_.read();
    uint8_t bit7 = val >> 7u;
    val = (val << 1u) | (uint8_t)(f_.carry);
    a_.write(val);
    f_.carry = bit7;

    f_.zero = false;
    f_.subtract = false;
    f_.half_carry = false;
}

void CPU::di() {
    // TODO: Interrupt is disabled after the instruction AFTER DI is executed.
    interrupt_enabled_ = false;
}

void CPU::ei() {
    // TODO: Interrupt is enabled after the instruction AFTER DI is executed.
    interrupt_enabled_ = true;
}

void CPU::call(Operand lhs, Operand rhs) {
    bool should_jump;

    switch (lhs) {
        // One operand
        case Operand::Imm16: should_jump = true; break;
        // Two operands
        case Operand::FlagC: should_jump = f_.carry; break;
        case Operand::FlagNC: should_jump = !f_.carry; break;
        case Operand::FlagZ: should_jump = f_.zero; break;
        case Operand::FlagNZ: should_jump = !f_.zero; break;
    }

    if (should_jump) {
        // Push current PC onto stack
        push_to_stack(pc_);
        pc_ = imm16_.read();
    }
}

void CPU::ret() {
    pc_ = pop_from_stack();
}

void CPU::push(Operand lhs) {
    auto value = get_operand16(lhs);
    DCHECK(value);
    push_to_stack((*value)->read());
}

void CPU::pop(Operand lhs) {
    auto value = get_operand16(lhs);
    DCHECK(value);
    (*value)->write(pop_from_stack());
}

void CPU::push_to_stack(uint16_t value) {
    // MSB first into SP - 1
    sp_.write(sp_.read() - 1);
    mem_->write(sp_.read(), value >> 8u);

    // Then LSB into SP - 2
    sp_.write(sp_.read() - 1);
    mem_->write(sp_.read(), value & 0x00FFu);
}

uint16_t CPU::pop_from_stack() {
    uint16_t value;

    // LSB first from SP.
    value = mem_->read(sp_.read());
    sp_.write(sp_.read() + 1);

    // Then MSB from SP + 1.
    value |= mem_->read(sp_.read()) << 8;
    sp_.write(sp_.read() + 1);

    return value;
}

void CPU::inc(Operand lhs) {
    auto op8 = get_operand8(lhs);
    if (op8) {
        uint8_t new_value = (*op8)->read() + 1;
        (*op8)->write(new_value);

        LOG(ERROR) << "Half carry flag not implemented";
        f_.subtract = false;
        f_.zero = (new_value == 0);

        return;
    }

    auto op16 = get_operand16(lhs);
    DCHECK(op16);
    (*op16)->write((*op16)->read() + 1);
}

void CPU::ldi(Operand lhs, Operand rhs) {
    if (lhs == Operand::PtrHL && rhs == Operand::A) {
        ptr_hl_.write(a_.read());
        hl_.write(hl_.read() + 1);
        return;
    }

    if (lhs == Operand::A && rhs == Operand::PtrHL) {
        a_.write(ptr_hl_.read());
        hl_.write(hl_.read() + 1);
        return;
    }

    DCHECK(false);
}

void CPU::or_(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t new_value = a_.read() | ((*op8)->read());
    a_.write(new_value);

    f_.carry = false;
    f_.half_carry = false;
    f_.subtract = false;
    // TODO: correct?
    f_.zero = (new_value == 0);
}

void CPU::and_(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t new_value = a_.read() & ((*op8)->read());
    a_.write(new_value);

    f_.carry = false;
    f_.half_carry = true;
    f_.subtract = false;
    // TODO: correct?
    f_.zero = (new_value == 0);
}

void CPU::dec(Operand lhs) {
    auto op8 = get_operand8(lhs);
    if (op8) {
        uint8_t new_value = (*op8)->read() - 1;
        (*op8)->write(new_value);

        LOG(ERROR) << "Half carry flag not implemented";
        f_.subtract = true;
        f_.zero = (new_value == 0);

        return;
    }

    auto op16 = get_operand16(lhs);
    DCHECK(op16);
    (*op16)->write((*op16)->read() - 1);
}

void CPU::xor_(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t new_value = a_.read() ^ ((*op8)->read());
    a_.write(new_value);

    f_.carry = false;
    f_.half_carry = false;
    f_.subtract = false;
    // TODO: correct?
    f_.zero = (new_value == 0);
}

void CPU::add(Operand lhs, Operand rhs) {
    if (lhs == Operand::A) {
        auto op8 = get_operand8(rhs);
        DCHECK(op8);

        uint8_t new_value = a_.read() + (*op8)->read();
        a_.write(new_value);

        LOG(ERROR) << "Carry flag not implemented";
        LOG(ERROR) << "Half-carry flag not implemented";
        f_.subtract = false;
        f_.zero = (new_value == 0);

        return;
    }

    if (lhs == Operand::HL) {
        auto op16 = get_operand16(rhs);
        DCHECK(op16);

        hl_.write(hl_.read() + (*op16)->read());

        LOG(ERROR) << "Carry flag not implemented";
        LOG(ERROR) << "Half-carry flag not implemented";
        f_.subtract = false;

        return;
    }

    if (lhs == Operand::SP && rhs == Operand::Imm8Sign) {
        sp_.write(sp_.read() + imm8sign_.read());

        LOG(ERROR) << "Carry flag not implemented";
        LOG(ERROR) << "Half-carry flag not implemented";
        f_.subtract = false;
        f_.zero = false;
    }

    DCHECK(false);
}

void CPU::ldd(Operand lhs, Operand rhs) {
    if (lhs == Operand::PtrHL && rhs == Operand::A) {
        ptr_hl_.write(a_.read());
        hl_.write(hl_.read() - 1);
        return;
    }

    if (lhs == Operand::A && rhs == Operand::PtrHL) {
        a_.write(ptr_hl_.read());
        hl_.write(hl_.read() - 1);
        return;
    }

    DCHECK(false);
}

void CPU::sub(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t a_value = a_.read(),
            lhs_value = (*op8)->read();

    // Carry occurs of a_value is smaller than lhs_value.
    f_.carry = (a_value < lhs_value);
    LOG(ERROR) << "Half-carry flag not implemented";
    f_.subtract = true;

    uint8_t new_value = a_.read() - (*op8)->read();
    f_.zero = (new_value == 0);
    a_.write(new_value);
}

void CPU::srl(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t new_value = (*op8)->read();

    // Carry flag contains the value of bit 0
    f_.carry = (new_value & 0x1u);

    new_value >>= 1u;

    f_.half_carry = false;
    f_.subtract = false;
    f_.zero = (new_value == 0);

    (*op8)->write(new_value);
}

void CPU::rr(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t new_value = (*op8)->read();
    uint8_t old_carry = f_.carry;

    // Carry flag contains the value of bit 0
    f_.carry = (new_value & 0x1u);
    new_value = (new_value >> 1u) | (old_carry << 7u);

    f_.half_carry = false;
    f_.subtract = false;
    f_.zero = (new_value == 0);

    (*op8)->write(new_value);
}

}  // namespace cpu
