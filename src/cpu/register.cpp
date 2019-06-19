#include "cpu/register.h"

namespace cpu {

uint8_t Register::get_msb(uint16_t value) { return (value >> 8); }
uint8_t Register::get_lsb(uint16_t value) { return (value & 0b11111111); }

void Register::set_msb(uint8_t src, uint16_t *dest) {
    *dest = ((*dest) & 0b0000000011111111) | (static_cast<uint16_t>(src) << 8);
}

void Register::set_lsb(uint8_t src, uint16_t *dest) {
    *dest = ((*dest) & 0b1111111100000000) | static_cast<uint16_t>(src);
}

uint8_t Register::a() { return get_msb(af_); }
uint8_t Register::f() { return get_lsb(af_); }
uint16_t Register::af() { return af_; }

uint8_t Register::b() { return get_msb(bc_); }
uint8_t Register::c() { return get_lsb(bc_); }
uint16_t Register::bc() { return bc_; }

uint8_t Register::d() { return get_msb(de_); }
uint8_t Register::e() { return get_lsb(de_); }
uint16_t Register::de() { return de_; }

uint8_t Register::h() { return get_msb(hl_); }
uint8_t Register::l() { return get_lsb(hl_); }
uint16_t Register::hl() { return hl_; }

uint16_t Register::sp() { return sp_; }
uint16_t Register::pc() { return pc_; }

void Register::set_a(uint8_t value) { return set_msb(value, &af_); }
void Register::set_f(uint8_t value) { return set_lsb(value, &af_); }
void Register::set_af(uint16_t value) { af_ = value; }

void Register::set_b(uint8_t value) { return set_msb(value, &bc_); }
void Register::set_c(uint8_t value) { return set_lsb(value, &bc_); }
void Register::set_bc(uint16_t value) { bc_ = value; }

void Register::set_d(uint8_t value) { return set_msb(value, &de_); }
void Register::set_e(uint8_t value) { return set_lsb(value, &de_); }
void Register::set_de(uint16_t value) { de_ = value; }

void Register::set_h(uint8_t value) { return set_msb(value, &hl_); }
void Register::set_l(uint8_t value) { return set_lsb(value, &hl_); }
void Register::set_hl(uint16_t value) { hl_ = value; }

void Register::set_sp(uint16_t value) { sp_ = value; }
void Register::set_pc(uint16_t value) { pc_ = value; }

} // namespace cpu
