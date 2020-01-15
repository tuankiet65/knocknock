#include "cpu/operands.h"

#include <fmt/format.h>
#include <glog/logging.h>

namespace cpu {

namespace {

const uint8_t ZERO_MASK = 0b10000000;
const uint8_t SUBTRACT_MASK = 0b01000000;
const uint8_t HALF_CARRY_MASK = 0b00100000;
const uint8_t CARRY_MASK = 0b00010000;

}  // namespace

Register8::Register8(std::string name) : name_(name) {}
uint8_t Register8::read() const {
    return value_;
}
void Register8::write(uint8_t value) {
    value_ = value;
}
std::string Register8::name() const {
    return name_;
}

FlagRegister::FlagRegister(std::string name) : Register8::Register8(name){};
void FlagRegister::set(FlagRegister::Flag flag) {
    uint8_t value = this->read();

    switch (flag) {
        case FlagRegister::Flag::Zero: value |= ZERO_MASK; break;
        case FlagRegister::Flag::Subtract: value |= SUBTRACT_MASK; break;
        case FlagRegister::Flag::HalfCarry: value |= HALF_CARRY_MASK; break;
        case FlagRegister::Flag::Carry: value |= CARRY_MASK; break;
        default: DCHECK(false) << "Unknown flag enum"; break;
    }

    this->write(value);
}

void FlagRegister::clear(FlagRegister::Flag flag) {
    uint8_t value = this->read();

    switch (flag) {
        case FlagRegister::Flag::Zero: value &= ~ZERO_MASK; break;
        case FlagRegister::Flag::Subtract: value &= ~SUBTRACT_MASK; break;
        case FlagRegister::Flag::HalfCarry: value &= ~HALF_CARRY_MASK; break;
        case FlagRegister::Flag::Carry: value &= ~CARRY_MASK; break;
        default: DCHECK(false) << "Unknown flag enum"; break;
    }

    this->write(value);
}

bool FlagRegister::get(FlagRegister::Flag flag) const {
    uint8_t value = read();

    switch (flag) {
        case FlagRegister::Flag::Zero: return value & ZERO_MASK;
        case FlagRegister::Flag::Subtract: return value & SUBTRACT_MASK;
        case FlagRegister::Flag::HalfCarry: return value & HALF_CARRY_MASK;
        case FlagRegister::Flag::Carry: return value & CARRY_MASK;
    }

    DCHECK(false) << "Unknown flag enum";
    return false;
}

Register8Sign::Register8Sign(std::string name) : name_(name) {}
int8_t Register8Sign::read() const {
    return value_;
}
void Register8Sign::write(int8_t value) {
    value_ = value;
}
std::string Register8Sign::name() const {
    return name_;
}

Register16::Register16(std::string name) : name_(name) {}
uint16_t Register16::read() const {
    return value_;
}
void Register16::write(uint16_t value) {
    value_ = value;
}
std::string Register16::name() const {
    return name_;
}

Register16Mirror::Register16Mirror(Register8 *msb, Register8 *lsb)
    : msb_(msb), lsb_(lsb) {}

uint16_t Register16Mirror::read() const {
    uint16_t result = msb_->read();
    result = (result << 8) | lsb_->read();

    return result;
}
void Register16Mirror::write(uint16_t value) {
    msb_->write(value >> 8);
    lsb_->write(value & 0b0000000011111111);
}
std::string Register16Mirror::name() const {
    return msb_->name() + lsb_->name();
}

Immediate8::Immediate8(uint8_t value) : value_(value) {}
uint8_t Immediate8::read() const {
    return value_;
}
void Immediate8::write(uint8_t value) {
    DCHECK(false) << "Attempting to write into Immediate8";
}
std::string Immediate8::name() const {
    return fmt::format("{}", value_);
}

Immediate16::Immediate16(uint16_t value) : value_(value) {}
uint16_t Immediate16::read() const {
    return value_;
}
void Immediate16::write(uint16_t value) {
    DCHECK(false) << "Attempting to write into Immediate16";
}
std::string Immediate16::name() const {
    return fmt::format("{}", value_);
}

Memory8::Memory8(memory::Memory *mem, const Operand16 &addr)
    : mem_(mem), addr_(addr) {}
uint8_t Memory8::read() const {
    return mem_->read(addr_.read());
}
void Memory8::write(uint8_t value) {
    mem_->write(addr_.read(), value);
}
std::string Memory8::name() const {
    return fmt::format("({})", addr_.name());
}

Memory16::Memory16(memory::Memory *mem, const Operand16 &addr)
    : mem_(mem), addr_(addr) {}
uint16_t Memory16::read() const {
    return mem_->read16(addr_.read());
}
void Memory16::write(uint16_t value) {
    mem_->write16(addr_.read(), value);
}
std::string Memory16::name() const {
    return fmt::format("({})", addr_.name());
}

}  // namespace cpu
