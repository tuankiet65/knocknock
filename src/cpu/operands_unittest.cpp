#include "cpu/operands.h"

#include <catch2/catch.hpp>
#include <random>

#include "memory/test_memory.h"

TEST_CASE("Register8", "[cpu][operands]") {
    cpu::Register8 reg8;

    reg8.write(0x69);
    REQUIRE(reg8.read() == 0x69);
}

TEST_CASE("Register16", "[cpu][operands]") {
    cpu::Register16 reg16;

    reg16.write(0x1234);
    REQUIRE(reg16.read() == 0x1234);
}

TEST_CASE("Register16Mirror", "[cpu][operands]") {
    cpu::Register8 msb, lsb;
    cpu::Register16Mirror reg16(&msb, &lsb);

    msb.write(0x12);
    lsb.write(0x34);
    REQUIRE(msb.read() == 0x12);
    REQUIRE(lsb.read() == 0x34);
    REQUIRE(reg16.read() == 0x1234);

    reg16.write(0x3344);
    REQUIRE(reg16.read() == 0x3344);
    REQUIRE(msb.read() == 0x33);
    REQUIRE(lsb.read() == 0x44);
}

TEST_CASE("Immediate8", "[cpu][operands]") {
    cpu::Immediate8 imm8(0x69);
    REQUIRE(imm8.read() == 0x69);
}

TEST_CASE("Immediate16", "[cpu][operands]") {
    cpu::Immediate16 imm16(0x9988);
    REQUIRE(imm16.read() == 0x9988);
}

TEST_CASE("Memory8", "[cpu][operands]") {
    memory::TestMemory ram;
    ram.write(0x1234, 0x69);

    cpu::Memory8 mem8(&ram, 0x1234);
    REQUIRE(mem8.read() == 0x69);
}

TEST_CASE("Memory16", "[cpu][operands]") {
    memory::TestMemory ram;
    ram.write16(0x1234, 0x6942);

    cpu::Memory16 mem16(&ram, 0x1234);
    REQUIRE(mem16.read() == 0x6942);
}
