#include <catch2/catch.hpp>

#include "cpu/register.h"

TEST_CASE("AF register", "[cpu]") {
    cpu::Register reg;

    reg.set_a(0x01);
    CHECK(reg.a() == 0x01);
    reg.set_a(0x99);
    CHECK(reg.a() == 0x99);

    reg.set_f(0x01);
    CHECK(reg.f() == 0x01);
    reg.set_f(0x99);
    CHECK(reg.f() == 0x99);

    reg.set_a(0x01);
    reg.set_f(0xff);
    CHECK(reg.af() == 0x01ff);
    reg.set_f(0x00);
    CHECK(reg.af() == 0x0100);
    reg.set_a(0xff);
    CHECK(reg.af() == 0xff00);

    reg.set_af(0x1234);
    CHECK(reg.af() == 0x1234);
    CHECK(reg.a() == 0x12);
    CHECK(reg.f() == 0x34);
}

TEST_CASE("BC register", "[cpu]") {
    cpu::Register reg;

    reg.set_b(0x01);
    CHECK(reg.b() == 0x01);
    reg.set_b(0x99);
    CHECK(reg.b() == 0x99);

    reg.set_c(0x01);
    CHECK(reg.c() == 0x01);
    reg.set_c(0x99);
    CHECK(reg.c() == 0x99);

    reg.set_b(0x01);
    reg.set_c(0xff);
    CHECK(reg.bc() == 0x01ff);
    reg.set_c(0x00);
    CHECK(reg.bc() == 0x0100);
    reg.set_b(0xff);
    CHECK(reg.bc() == 0xff00);

    reg.set_bc(0x1234);
    CHECK(reg.bc() == 0x1234);
    CHECK(reg.b() == 0x12);
    CHECK(reg.c() == 0x34);
}

TEST_CASE("DE register", "[cpu]") {
    cpu::Register reg;

    reg.set_d(0x01);
    CHECK(reg.d() == 0x01);
    reg.set_d(0x99);
    CHECK(reg.d() == 0x99);

    reg.set_e(0x01);
    CHECK(reg.e() == 0x01);
    reg.set_e(0x99);
    CHECK(reg.e() == 0x99);

    reg.set_d(0x01);
    reg.set_e(0xff);
    CHECK(reg.de() == 0x01ff);
    reg.set_e(0x00);
    CHECK(reg.de() == 0x0100);
    reg.set_d(0xff);
    CHECK(reg.de() == 0xff00);

    reg.set_de(0x1234);
    CHECK(reg.de() == 0x1234);
    CHECK(reg.d() == 0x12);
    CHECK(reg.e() == 0x34);
}

TEST_CASE("HL register", "[cpu]") {
    cpu::Register reg;

    reg.set_h(0x01);
    CHECK(reg.h() == 0x01);
    reg.set_h(0x99);
    CHECK(reg.h() == 0x99);

    reg.set_l(0x01);
    CHECK(reg.l() == 0x01);
    reg.set_l(0x99);
    CHECK(reg.l() == 0x99);

    reg.set_h(0x01);
    reg.set_l(0xff);
    CHECK(reg.hl() == 0x01ff);
    reg.set_l(0x00);
    CHECK(reg.hl() == 0x0100);
    reg.set_h(0xff);
    CHECK(reg.hl() == 0xff00);

    reg.set_hl(0x1234);
    CHECK(reg.hl() == 0x1234);
    CHECK(reg.h() == 0x12);
    CHECK(reg.l() == 0x34);
}

TEST_CASE("SP register", "[cpu]") {
    cpu::Register reg;

    reg.set_sp(0x1234);
    CHECK(reg.sp() == 0x1234);

    reg.set_sp(0xf00f);
    CHECK(reg.sp() == 0xf00f);
}

TEST_CASE("PC register", "[cpu]") {
    cpu::Register reg;

    reg.set_pc(0x1234);
    CHECK(reg.pc() == 0x1234);

    reg.set_pc(0xf00f);
    CHECK(reg.pc() == 0xf00f);
}
