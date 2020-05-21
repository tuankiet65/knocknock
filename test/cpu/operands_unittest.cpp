#include <catch2/catch.hpp>

#include <knocknock/cpu/operands.h>
#include <knocknock/memory/test_memory.h>

TEST_CASE("Register8", "[cpu][operands]") {
    cpu::Register8 reg8;

    reg8.write(0x69);
    REQUIRE(reg8.read() == 0x69);
}

TEST_CASE("FlagRegister", "[cpu][operands]") {
    cpu::FlagRegister f;

    SECTION("Raw set, porcelain get") {
        f.write(0b01010000);
        REQUIRE(!f.zero);
        REQUIRE(f.subtract);
        REQUIRE(!f.half_carry);
        REQUIRE(f.carry);

        f.write(0b10100000);
        REQUIRE(f.zero);
        REQUIRE(!f.subtract);
        REQUIRE(f.half_carry);
        REQUIRE(!f.carry);
    }

    SECTION("Porcelain set, raw get") {
        f.zero = true;
        f.subtract = true;
        f.half_carry = true;
        f.carry = true;
        REQUIRE(f.read() == 0b11110000);

        f.zero = false;
        f.subtract = false;
        f.half_carry = false;
        f.carry = false;
        REQUIRE(f.read() == 0b00000000);
    }
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
    cpu::Immediate8 imm8(69);

    REQUIRE(imm8.read() == 69);
}

TEST_CASE("Immediate16", "[cpu][operands]") {
    cpu::Immediate16 imm16(9988);

    REQUIRE(imm16.read() == 9988);
}

TEST_CASE("Memory8", "[cpu][operands]") {
    memory::TestMemory mem;
    mem.write(1234, 0x69);

    REQUIRE(mem.read(1234) == 0x69);

    SECTION("Memory8 + Immediate16") {
        cpu::Immediate16 addr(1234);

        REQUIRE(addr.read() == 1234);

        cpu::Memory8 mem8(&mem, addr);
        REQUIRE(mem8.read() == 0x69);
    }

    SECTION("Memory8 + Register16") {
        cpu::Register16 addr;
        addr.write(1234);

        REQUIRE(addr.read() == 1234);

        cpu::Memory8 mem8(&mem, addr);
        REQUIRE(mem8.read() == 0x69);
    }
}

TEST_CASE("Memory8HiMem", "[cpu][operands]") {
    memory::TestMemory mem;
    mem.write(0xff10, 0x69);

    REQUIRE(mem.read(0xff10) == 0x69);

    SECTION("Memory8HiMem + Immediate8") {
        cpu::Immediate8 addr(0x10);

        REQUIRE(addr.read() == 0x10);

        cpu::Memory8HiMem mem8(&mem, addr);
        REQUIRE(mem8.read() == 0x69);
    }

    SECTION("Memory8 + Register8") {
        cpu::Register8 addr;
        addr.write(0x10);

        REQUIRE(addr.read() == 0x10);

        cpu::Memory8HiMem mem8(&mem, addr);
        REQUIRE(mem8.read() == 0x69);
    }
}

TEST_CASE("Memory16", "[cpu][operands]") {
    memory::TestMemory mem;
    mem.write16(1234, 0x6942);

    REQUIRE(mem.read16(1234) == 0x6942);

    SECTION("Memory16 + Immediate16") {
        cpu::Immediate16 addr(1234);
        REQUIRE(addr.read() == 1234);

        cpu::Memory16 mem16(&mem, addr);
        REQUIRE(mem16.read() == 0x6942);
    }

    SECTION("Memory16 + Register16") {
        cpu::Register16 addr;
        addr.write(1234);
        REQUIRE(addr.read() == 1234);

        cpu::Memory16 mem16(&mem, addr);
        REQUIRE(mem16.read() == 0x6942);
    }
}
