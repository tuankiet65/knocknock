#include "cpu/operands.h"

#include "glog/logging.h"

namespace cpu {

Register8::Register8() {}
uint8_t Register8::read() const {
    return value_;
}
void Register8::write(uint8_t value) {
    value_ = value;
}

Register16::Register16() {}
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
void Immediate8::write(uint8_t value) {
    DCHECK(false) << "Attempting to write into Immediate8";
}

Immediate16::Immediate16(uint16_t value) : value_(value) {}
uint16_t Immediate16::read() const {
    return value_;
}
void Immediate16::write(uint16_t value) {
    DCHECK(false) << "Attempting to write into Immediate16";
}

Memory8::Memory8(memory::Memory *mem, memory::MemoryAddr addr)
    : mem_(mem), addr_(addr) {}
uint8_t Memory8::read() const {
    return mem_->read(addr_);
}
void Memory8::write(uint8_t value) {
    mem_->write(addr_, value);
}

Memory16::Memory16(memory::Memory *mem, memory::MemoryAddr addr)
    : mem_(mem), addr_(addr) {}
uint16_t Memory16::read() const {
    return mem_->read16(addr_);
}
void Memory16::write(uint16_t value) {
    mem_->write16(addr_, value);
}

}  // namespace cpu
