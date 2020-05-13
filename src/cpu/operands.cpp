#include "cpu/operands.h"

#include <glog/logging.h>

#include "memory/regions.h"

namespace cpu {

namespace {

constexpr uint8_t ZERO_MASK = 1 << 7;
constexpr uint8_t SUBTRACT_MASK = 1 << 6;
constexpr uint8_t HALF_CARRY_MASK = 1 << 5;
constexpr uint8_t CARRY_MASK = 1 << 4;

}  // namespace

Register8::Register8() : value_(0) {}
uint8_t Register8::read() const {
    return value_;
}
void Register8::write(uint8_t value) {
    value_ = value;
}

FlagRegister::FlagRegister()
    : zero(false), subtract(false), half_carry(false), carry(false) {}

uint8_t FlagRegister::read() const {
    uint8_t result = 0;

    if (zero)
        result |= ZERO_MASK;
    if (subtract)
        result |= SUBTRACT_MASK;
    if (half_carry)
        result |= HALF_CARRY_MASK;
    if (carry)
        result |= CARRY_MASK;

    return result;
}

void FlagRegister::write(uint8_t value) {
    zero = value & ZERO_MASK;
    subtract = value & SUBTRACT_MASK;
    half_carry = value & HALF_CARRY_MASK;
    carry = value & CARRY_MASK;
}

Register8Sign::Register8Sign() : value_(0) {}
int8_t Register8Sign::read() const {
    return value_;
}
void Register8Sign::write(int8_t value) {
    value_ = value;
}

Register16::Register16() : value_(0) {}
uint16_t Register16::read() const {
    return value_;
}
void Register16::write(uint16_t value) {
    value_ = value;
}

Register16Mirror::Register16Mirror(Operand8 *msb, Operand8 *lsb)
    : msb_(msb), lsb_(lsb) {}

uint16_t Register16Mirror::read() const {
    uint16_t result = msb_->read();
    result = (result << 8) | lsb_->read();

    return result;
}
void Register16Mirror::write(uint16_t value) {
    msb_->write(value >> 8u);
    lsb_->write(value & 0x00FFu);
}

Immediate8::Immediate8(uint8_t value) : value_(value) {}
uint8_t Immediate8::read() const {
    return value_;
}
void Immediate8::write([[maybe_unused]] uint8_t value) {
    DCHECK(false) << "Attempting to write into Immediate8";
}

Immediate16::Immediate16(uint16_t value) : value_(value) {}
uint16_t Immediate16::read() const {
    return value_;
}
void Immediate16::write([[maybe_unused]] uint16_t value) {
    DCHECK(false) << "Attempting to write into Immediate16";
}

Memory8::Memory8(memory::Memory *mem, const Operand16 &addr)
    : mem_(mem), addr_(addr) {}
uint8_t Memory8::read() const {
    return mem_->read(addr_.read());
}
void Memory8::write(uint8_t value) {
    mem_->write(addr_.read(), value);
}

Memory8HiMem::Memory8HiMem(memory::Memory *mem, const Operand8 &addr)
    : mem_(mem), addr_(addr) {}
uint8_t Memory8HiMem::read() const {
    return mem_->read(0xff00 + addr_.read());
}
void Memory8HiMem::write(uint8_t value) {
    mem_->write(0xff00 + addr_.read(), value);
}

Memory16::Memory16(memory::Memory *mem, const Operand16 &addr)
    : mem_(mem), addr_(addr) {}
uint16_t Memory16::read() const {
    return mem_->read16(addr_.read());
}
void Memory16::write(uint16_t value) {
    mem_->write16(addr_.read(), value);
}

}  // namespace cpu
