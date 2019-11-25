#include "cpu/operands.h"

#include <catch2/catch.hpp>
#include <random>

#include "memory/test_memory.h"

TEST_CASE("Register8", "[cpu][operands]") {
    cpu::Register8 reg8("A");

    REQUIRE(reg8.name() == "A");

    reg8.write(0x69);
    REQUIRE(reg8.read() == 0x69);
}

TEST_CASE("FlagRegister", "[cpu][operands]") {
    cpu::FlagRegister f("F");

    REQUIRE(f.name() == "F");

    // All off
    f.write(0b00000000);
    REQUIRE(f.read() == 0b00000000);
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Zero));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Subtract));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::HalfCarry));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Carry));

    // All on
    f.write(0b11110000);
    REQUIRE(f.read() == 0b11110000);
    REQUIRE(f.get(cpu::FlagRegister::Flag::Zero));
    REQUIRE(f.get(cpu::FlagRegister::Flag::Subtract));
    REQUIRE(f.get(cpu::FlagRegister::Flag::HalfCarry));
    REQUIRE(f.get(cpu::FlagRegister::Flag::Carry));

    // All off
    f.clear(cpu::FlagRegister::Flag::Zero);
    f.clear(cpu::FlagRegister::Flag::Subtract);
    f.clear(cpu::FlagRegister::Flag::HalfCarry);
    f.clear(cpu::FlagRegister::Flag::Carry);
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Zero));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Subtract));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::HalfCarry));
    REQUIRE(!f.get(cpu::FlagRegister::Flag::Carry));

    // All on
    f.set(cpu::FlagRegister::Flag::Zero);
    f.set(cpu::FlagRegister::Flag::Subtract);
    f.set(cpu::FlagRegister::Flag::HalfCarry);
    f.set(cpu::FlagRegister::Flag::Carry);
    REQUIRE(f.get(cpu::FlagRegister::Flag::Zero));
    REQUIRE(f.get(cpu::FlagRegister::Flag::Subtract));
    REQUIRE(f.get(cpu::FlagRegister::Flag::HalfCarry));
    REQUIRE(f.get(cpu::FlagRegister::Flag::Carry));
}

TEST_CASE("Register16", "[cpu][operands]") {
    cpu::Register16 reg16("BC");

    REQUIRE(reg16.name() == "BC");

    reg16.write(0x1234);
    REQUIRE(reg16.read() == 0x1234);
}

TEST_CASE("Register16Mirror", "[cpu][operands]") {
    cpu::Register8 msb("B"), lsb("C");
    cpu::Register16Mirror reg16(&msb, &lsb);

    REQUIRE(msb.name() == "B");
    REQUIRE(lsb.name() == "C");
    REQUIRE(reg16.name() == "BC");

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

    REQUIRE(imm8.name() == "69");
    REQUIRE(imm8.read() == 69);
}

TEST_CASE("Immediate16", "[cpu][operands]") {
    cpu::Immediate16 imm16(9988);

    REQUIRE(imm16.name() == "9988");
    REQUIRE(imm16.read() == 9988);
}

TEST_CASE("Memory8", "[cpu][operands]") {
    memory::TestMemory mem;
    mem.write(1234, 0x69);

    REQUIRE(mem.read(1234) == 0x69);

    SECTION("Memory8 + Immediate16") {
        cpu::Immediate16 addr(1234);

        REQUIRE(addr.name() == "1234");
        REQUIRE(addr.read() == 1234);

        cpu::Memory8 mem8(&mem, addr);

        REQUIRE(mem8.name() == "(1234)");
        REQUIRE(mem8.read() == 0x69);
    }

    SECTION("Memory8 + Register16") {
        cpu::Register16 addr("NN");
        addr.write(1234);

        REQUIRE(addr.name() == "NN");
        REQUIRE(addr.read() == 1234);

        cpu::Memory8 mem8(&mem, addr);

        REQUIRE(mem8.name() == "(NN)");
        REQUIRE(mem8.read() == 0x69);
    }
}

TEST_CASE("Memory16", "[cpu][operands]") {
    memory::TestMemory mem;
    mem.write16(1234, 0x6942);

    REQUIRE(mem.read16(1234) == 0x6942);

    SECTION("Memory16 + Immediate16") {
        cpu::Immediate16 addr(1234);

        REQUIRE(addr.name() == "1234");
        REQUIRE(addr.read() == 1234);

        cpu::Memory16 mem16(&mem, addr);

        REQUIRE(mem16.name() == "(1234)");
        REQUIRE(mem16.read() == 0x6942);
    }

    SECTION("Memory16 + Register16") {
        cpu::Register16 addr("NN");
        addr.write(1234);

        REQUIRE(addr.name() == "NN");
        REQUIRE(addr.read() == 1234);

        cpu::Memory16 mem16(&mem, addr);

        REQUIRE(mem16.name() == "(NN)");
        REQUIRE(mem16.read() == 0x6942);
    }
}
