#pragma once

#include <cstdint>

namespace cpu {

class Register {
private:
    uint16_t af_, bc_, de_, hl_, sp_, pc_;

    uint8_t get_msb(uint16_t value);
    uint8_t get_lsb(uint16_t value);
    void set_msb(uint8_t src, uint16_t *dest);
    void set_lsb(uint8_t src, uint16_t *dest);

public:
    Register(): af_(0), bc_(0), de_(0), hl_(0), sp_(0), pc_(0) {}

    // Getters
    uint8_t a();
    uint8_t f();
    uint16_t af();

    uint8_t b();
    uint8_t c();
    uint16_t bc();

    uint8_t d();
    uint8_t e();
    uint16_t de();

    uint16_t sp();
    uint16_t pc();

    // Setters
    void set_a(uint8_t value);
    void set_f(uint8_t value);
    void set_af(uint16_t value);

    void set_b(uint8_t value);
    void set_c(uint8_t value);
    void set_bc(uint16_t value);

    void set_d(uint8_t value);
    void set_e(uint8_t value);
    void set_de(uint16_t value);

    void set_sp(uint16_t value);
    void set_pc(uint16_t value);
};

} // namespace cpu
