#include "cpu/operands.h"

#include <fmt/format.h>
#include <glog/logging.h>

namespace cpu {

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
