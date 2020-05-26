#include "knocknock/cpu/cpu.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <iostream>

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
        default: return {};
    }
}

std::optional<Operand16 *> CPU::get_operand16(Operand operand) {
    switch (operand) {
        case Operand::AF: return &af_;
        case Operand::BC: return &bc_;
        case Operand::DE: return &de_;
        case Operand::HL: return &hl_;
        case Operand::SP: return &sp_;
        case Operand::Imm16: return &imm16_;
        default: return {};
    }
}

void CPU::tick() {

    decoder_.step();

    if (!decoder_.decoded_instruction().has_value()) {
        return;
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
        case Opcode::RET: ret(inst.lhs()); break;
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
        case Opcode::RRA: rra(); break;
        case Opcode::ADC: adc(inst.lhs(), inst.rhs()); break;
        case Opcode::SBC: sbc(inst.lhs(), inst.rhs()); break;
        case Opcode::CPL: cpl(); break;
        case Opcode::SCF: scf(); break;
        case Opcode::CCF: ccf(); break;
        case Opcode::RL: rl(inst.lhs()); break;
        case Opcode::RLC: rlc(inst.lhs()); break;
        case Opcode::RRC: rrc(inst.lhs()); break;
        case Opcode::RRCA: rrca(); break;
        case Opcode::SLA: sla(inst.lhs()); break;
        case Opcode::SRA: sra(inst.lhs()); break;
        case Opcode::RST: rst(inst.lhs()); break;
        case Opcode::BIT: bit(inst.lhs(), inst.rhs()); break;
        case Opcode::RES: res(inst.lhs(), inst.rhs()); break;
        case Opcode::SET: set(inst.lhs(), inst.rhs()); break;
        case Opcode::DAA: daa(); break;
        case Opcode::LDHL: ldhl(inst.lhs(), inst.rhs()); break;

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
        DCHECK(rhs == Operand::Imm16);
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
        DCHECK(rhs == Operand::Imm8Sign);
        pc_ += imm8sign_.read();
    }
}

void CPU::ld(Operand lhs, Operand rhs) {
    // Special exception for 0x08: LD (Imm16), SP
    // Load LSB of SP into Imm16 and MSB of SP into Imm16 + 1
    if (lhs == Operand::PtrImm16 && rhs == Operand::SP) {
        uint16_t addr = imm16_.read();

        mem_->write(addr, sp_.read() & 0x00FFu);
        mem_->write(addr + 1, sp_.read() >> 8u);
        return;
    }

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

    f_.zero = (x == y);
    f_.subtract = true;
    f_.half_carry = (low_nibble(x) < low_nibble(y));
    f_.carry = (x < y);
}

void CPU::swap(Operand lhs) {
    std::optional<Operand8 *> reg = get_operand8(lhs);
    DCHECK(reg.has_value());

    uint8_t val = (*reg)->read();
    uint8_t new_val = high_nibble(val) | (low_nibble(val) << 4u);
    (*reg)->write(new_val);

    f_.zero = (new_val == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = false;
}

void CPU::rlc(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t new_value = (value << 1u) | (value >> 7u);
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = (value >> 7u);
}

// Specialization of RLC for A register. RRCA set the zero flag to 0, unlike RLC
void CPU::rlca() {
    rlc(Operand::A);
    f_.zero = false;
}

void CPU::rl(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read(), carry = f_.carry;
    uint8_t new_value = (value << 1u) | carry;
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = (value >> 7u);
}

// Specialization of RLC for A register. RRCA set the zero flag to 0, unlike RLC
void CPU::rla() {
    rl(Operand::A);
    f_.zero = false;
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

void CPU::ret(Operand lhs) {
    bool should_return = false;

    switch (lhs) {
        case Operand::None: should_return = true; break;
        case Operand::FlagC: should_return = f_.carry; break;
        case Operand::FlagNC: should_return = !f_.carry; break;
        case Operand::FlagZ: should_return = f_.zero; break;
        case Operand::FlagNZ: should_return = !f_.zero; break;
    }

    if (should_return) {
        pc_ = pop_from_stack();
    }
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
    value |= mem_->read(sp_.read()) << 8u;
    sp_.write(sp_.read() + 1);

    return value;
}

void CPU::inc(Operand lhs) {
    auto op8 = get_operand8(lhs);
    if (op8) {
        uint8_t value = (*op8)->read();
        uint8_t new_value = value + 1;
        (*op8)->write(new_value);

        f_.zero = (new_value == 0);
        f_.subtract = false;
        // If the low nibble of value is 0b1111 then 0b1111 + 0b1 would generate
        // a carry bit.
        f_.half_carry = (low_nibble(value) == 0xFu);

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
    f_.zero = (new_value == 0);
}

void CPU::dec(Operand lhs) {
    auto op8 = get_operand8(lhs);
    if (op8) {
        uint8_t value = (*op8)->read();
        uint8_t new_value = value - 1;
        (*op8)->write(new_value);

        f_.zero = (new_value == 0);
        f_.subtract = true;
        // If the low nibble of value is 0b0000 then 0b0000 - 0b1 would generate
        // a borrow.
        f_.half_carry = (low_nibble(value) == 0x0u);

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
    f_.zero = (new_value == 0);
}

void CPU::add(Operand lhs, Operand rhs) {
    if (lhs == Operand::A) {
        auto op8 = get_operand8(rhs);
        DCHECK(op8);

        uint8_t x = a_.read(), y = (*op8)->read();
        uint8_t new_value = x + y;
        a_.write(new_value);

        f_.zero = (new_value == 0);
        f_.subtract = false;
        f_.half_carry = (low_nibble(x) + low_nibble(y) > 0xFu);
        //    x + y > 0xFF
        // => x     > 0xFF - y
        f_.carry = (x > (0xFF - y));

        return;
    }

    if (lhs == Operand::HL) {
        auto op16 = get_operand16(rhs);
        DCHECK(op16);

        uint16_t x = hl_.read(), y = (*op16)->read();
        hl_.write(x + y);

        f_.subtract = false;
        f_.half_carry = ((x & 0x0FFFu) + (y & 0x0FFFu) > 0x0FFFu);
        //    x + y > 0xFFFF
        // => x     > 0xFFFF - y
        f_.carry = (x > (0xFFFF - y));

        return;
    }

    if (lhs == Operand::SP && rhs == Operand::Imm8Sign) {
        uint16_t x = sp_.read();
        int8_t y = imm8sign_.read();

        sp_.write(x + y);

        f_.zero = false;
        f_.subtract = false;
        // The half_carry flag here signifies carry from bit 3 to 4, not 11
        // to 12.
        f_.half_carry = ((x & 0x000Fu) + low_nibble(y) > 0x000Fu);
        // The half_carry flag here signifies carry from bit 7 to 8, not 15
        // to 16.
        f_.carry = ((x & 0x00FFu) > (0x00FFu - (uint8_t)(y)));

        return;
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

    uint8_t x = a_.read(), y = (*op8)->read();
    uint8_t new_value = x - y;
    a_.write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = true;
    f_.half_carry = (low_nibble(x) < low_nibble(y));
    f_.carry = (x < y);
}

void CPU::srl(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t new_value = value >> 1u;
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    // Carry flag contains the value of bit 0
    f_.carry = (value & 0x1u);
}

void CPU::rr(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t carry = f_.carry;

    uint8_t new_value = (value >> 1u)     // shift the old value to the left
                        | (carry << 7u);  // and make the 7th bit the carry

    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    // Carry flag contains the value of bit 0
    f_.carry = (value & 0x1u);
}

void CPU::adc(Operand lhs, Operand rhs) {
    DCHECK(lhs == Operand::A);

    auto op8 = get_operand8(rhs);
    DCHECK(op8);

    uint8_t x = a_.read(), y = (*op8)->read(), carry = f_.carry;
    uint8_t new_value = x + y + carry;
    a_.write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = ((low_nibble(x) + low_nibble(y) + carry) > 0x0Fu);

    // There are two possible scenarios here:
    // * (x + y) overflows => (x + y + carry) overflows
    // * x + y == 0xff, which does not overflow, but if carry = 1 then
    //   x + y + carry == 0, which overflows.
    f_.carry = (x > (0xFFu - y)) || (x + y == 0xFFu && carry);
}

void CPU::sbc(Operand lhs, Operand rhs) {
    DCHECK(lhs == Operand::A);

    auto op8 = get_operand8(rhs);
    DCHECK(op8);

    uint8_t x = a_.read(), y = (*op8)->read(), carry = f_.carry;
    uint8_t new_value = x - y - carry;
    a_.write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = true;
    f_.half_carry = (low_nibble(x) < (low_nibble(y) + carry));

    // There are two possible scenarios here:
    // * (x - y) overflows => (x - y - carry) overflows.
    // * (x - y) == 0, which does not overflow, but (x - y - carry) would
    //   overflow if carry == 1.
    f_.carry = (x < y) || (x == y && carry);
}

void CPU::cpl() {
    a_.write(~a_.read());

    f_.subtract = true;
    f_.half_carry = true;
}

void CPU::scf() {
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = true;
}

void CPU::ccf() {
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = !f_.carry;
}

// Specialization of RR for A register. RRA set the zero flag to 0, unlike RR.
void CPU::rra() {
    rr(Operand::A);

    f_.zero = false;
}

void CPU::rrc(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t new_value = (value >> 1u) | ((value & 0x1u) << 7u);
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = (value & 0x1u);
}

// Specialization of RRC for A register. RRCA set the zero flag to 0, unlike RRC.
void CPU::rrca() {
    rrc(Operand::A);

    f_.zero = false;
}

void CPU::sla(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t new_value = value << 1u;
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = (value >> 7u);
}

void CPU::sra(Operand lhs) {
    auto op8 = get_operand8(lhs);
    DCHECK(op8);

    uint8_t value = (*op8)->read();
    uint8_t new_value = (value >> 1u) | ((value >> 7u) << 7u);
    (*op8)->write(new_value);

    f_.zero = (new_value == 0);
    f_.subtract = false;
    f_.half_carry = false;
    f_.carry = (value & 0x1u);
}

void CPU::rst(Operand lhs) {
    DCHECK(lhs == Operand::Imm8);

    push_to_stack(pc_);
    pc_ = imm8_.read();
}

void CPU::bit(Operand lhs, Operand rhs) {
    DCHECK(lhs == Operand::Imm8);

    auto op8 = get_operand8(rhs);
    DCHECK(op8);

    uint8_t x = imm8_.read(), y = (*op8)->read();

    f_.zero = !(y & (1u << x));
    f_.subtract = false;
    f_.half_carry = true;
}

void CPU::res(Operand lhs, Operand rhs) {
    DCHECK(lhs == Operand::Imm8);

    auto op8 = get_operand8(rhs);
    DCHECK(op8);

    uint8_t x = imm8_.read(), y = (*op8)->read();
    uint8_t new_value = y & (~(1u << x));
    (*op8)->write(new_value);
}

void CPU::set(Operand lhs, Operand rhs) {
    DCHECK(lhs == Operand::Imm8);

    auto op8 = get_operand8(rhs);
    DCHECK(op8);

    uint8_t x = imm8_.read(), y = (*op8)->read();
    uint8_t new_value = y | (1u << x);
    (*op8)->write(new_value);
}

void CPU::daa() {
    uint8_t a = a_.read();

    if (!f_.subtract) {
        if (f_.half_carry || ((a & 0x0f) > 0x09)) {
            // Explanation: a is an uint8_t, so if the below addition overflows
            // and f_.carry is false, then the condition (a > 0x9f) below fails,
            // which is not we're looking for when a is between 0xf0 and 0xff.
            // So we "abuse" the f_.carry flag to also indicate that the
            // addition will overflow, thus making sure the below condition is
            // true.
            f_.carry |= ((0xffu - a) < 0x06);

            a += 0x06;
        }

        if (f_.carry || (a > 0x9f)) {
            a += 0x60;
            f_.carry = true;
        }
    } else {
        if (f_.half_carry) {
            a -= 0x06;
        }

        if (f_.carry) {
            a -= 0x60;
        }
    }

    a_.write(a);

    f_.half_carry = false;
    f_.zero = (a == 0);
}

void CPU::ldhl(Instruction::Operand lhs, Instruction::Operand rhs) {
    DCHECK(lhs == Instruction::Operand::SP);
    DCHECK(rhs == Instruction::Operand::Imm8Sign);

    uint16_t x = sp_.read();
    int8_t y = imm8sign_.read();

    hl_.write(x + y);

    f_.zero = false;
    f_.subtract = false;
    // The half_carry flag here signifies carry from bit 3 to 4, not 11 to 12.
    f_.half_carry = ((x & 0x000Fu) + low_nibble(y) > 0x000Fu);
    // The half_carry flag here signifies carry from bit 7 to 8, not 15 to 16.
    f_.carry = ((x & 0x00FFu) > (0x00FFu - (uint8_t)(y)));
}

}  // namespace cpu
