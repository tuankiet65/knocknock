#include "cpu/operands.h"

#include <catch2/catch.hpp>
#include <random>

#include "memory/test_memory.h"

int random_int(int start, int end) {
    // https://stackoverflow.com/a/13445752
    static std::random_device dev;
    static std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(start, end);

    return dist(rng);
}

const int TESTS = 64;

TEST_CASE("Register8", "[cpu][operands]") {
    cpu::Register8 reg8;

    for (int i = 0; i < TESTS; ++i) {
        uint8_t value = random_int(0, 255);

        reg8.write(value);
        REQUIRE(reg8.read() == value);
    }
}

TEST_CASE("Register16", "[cpu][operands]") {
    cpu::Register16 reg16;

    for (int i = 0; i < TESTS; ++i) {
        uint16_t value = random_int(0, 65535);

        reg16.write(value);
        REQUIRE(reg16.read() == value);
    }
}

TEST_CASE("Register16Mirror", "[cpu][operands]") {
    cpu::Register8 msb, lsb;
    cpu::Register16Mirror reg16(&msb, &lsb);

    for (int i = 0; i < TESTS; ++i) {
        uint8_t val1 = random_int(0, 255), val2 = random_int(0, 255);

        msb.write(val1);
        lsb.write(val2);

        REQUIRE(reg16.read() == (val1 << 8 | val2));
    }

    for (int i = 0; i < TESTS; ++i) {
        uint16_t value = random_int(0, 65535);

        reg16.write(value);

        REQUIRE(msb.read() == (value >> 8));
        REQUIRE(lsb.read() == (value & 0b0000000011111111));
    }
}

TEST_CASE("Immediate8", "[cpu][operands]") {
    for (int i = 0; i < TESTS; ++i) {
        uint8_t value = random_int(0, 255);
        cpu::Immediate8 imm8(value);
        REQUIRE(imm8.read() == value);
    }
}

TEST_CASE("Immediate16", "[cpu][operands]") {
    for (int i = 0; i < TESTS; ++i) {
        uint16_t value = random_int(0, 65535);
        cpu::Immediate16 imm16(value);
        REQUIRE(imm16.read() == value);
    }
}

TEST_CASE("Memory8", "[cpu][operands]") {
    memory::TestMemory ram;

    for (int i = 0; i < TESTS; ++i) {
        const memory::MemoryAddr addr = 0x1234;

        uint8_t value = random_int(0, 255);
        ram.write(addr, value);

        cpu::Memory8 mem8(&ram, addr);
        REQUIRE(mem8.read() == value);
    }
}

TEST_CASE("Memory16", "[cpu][operands]") {
    memory::TestMemory ram;

    for (int i = 0; i < TESTS; ++i) {
        const memory::MemoryAddr addr = 0x1234;

        uint16_t value = random_int(0, 65535);
        ram.write16(addr, value);

        cpu::Memory16 mem16(&ram, addr);
        REQUIRE(mem16.read() == value);
    }
}
