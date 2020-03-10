#include "cpu/operands.h"

#include <glog/logging.h>

#include "memory/regions.h"

namespace cpu {

Register8::Register8() : value_(0) {}
uint8_t Register8::read() const {
    return value_;
}
void Register8::write(uint8_t value) {
    value_ = value;
}

FlagRegister::Flag::Flag(FlagRegister *reg, uint8_t mask)
    : reg_(reg), mask_(mask) {}

FlagRegister::Flag::operator bool() const {
    return (reg_->read() & mask_);
}

FlagRegister::Flag &FlagRegister::Flag::operator=(FlagRegister::Flag value) {
    *this = (bool)(value);
    return *this;
}

FlagRegister::Flag &FlagRegister::Flag::operator=(bool value) {
    uint8_t current_value = reg_->read();
    if (value) {
        // Turn on the bit.
        current_value |= mask_;
    } else {
        // Turn off the bit.
        current_value &= (~(mask_));
    }
    reg_->write(current_value);
    return *this;
}

FlagRegister::FlagRegister()
    : Register8::Register8(),
      zero(this, 1u << 7),
      subtract(this, 1u << 6),
      half_carry(this, 1u << 5),
      carry(this, 1u << 4) {}

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
