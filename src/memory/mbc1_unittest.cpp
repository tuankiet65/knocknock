#include <catch2/catch.hpp>

#include "memory/mbc1.h"
#include "memory/regions.h"
#include "memory/unittest_utils.h"

namespace memory {

namespace {

constexpr MemorySize RAM_BANK_SIZE = 0x2000;

void change_rom_bank(MBC1 *mem, uint8_t bank) {
    // First five bits of the bank number
    mem->write(0x2420, bank & 0b00011111);
    // Bit 6-5 of the bank number
    mem->write(0x4001, (bank >> 5) & 0b11);
}

void change_ram_bank(MBC1 *mem, uint8_t bank) {
    mem->write(0x4001, bank & 0b11);
}

}  // namespace

TEST_CASE("ROM Banking mode", "[memory][mbc1]") {
    // Generate a ROM with 64 banks, bank 0x00 filled with 0x01, bank 0x01
    // filled with 0x02, bank 0x10 filled with 0x10, bank 0x21 filled with 0x20.
    std::vector<MemoryValue> rom = testing::generate_test_rom(
        64, {{0x00, 0x01}, {0x01, 0x02}, {0x10, 0x10}, {0x21, 0x20}});

    // Then we create the MBC and load the generated ROM in.
    // 64 ROM banks, 1 RAM bank.
    MBC1 mem(rom.size(), RAM_BANK_SIZE);
    mem.load_rom(rom);

    // Change to ROM addressing mode.
    mem.write(0x6069, 0);

    // Check value in ROM bank 0.
    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    // Check value in ROM bank 0x01.
    change_rom_bank(&mem, 0x01);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));

    // Check value in ROM bank 0x10.
    change_rom_bank(&mem, 0x10);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x10));

    // We previously filled bank 0x21 to 0x20. If we attempt to read from
    // bank 0x20 now, it should read from bank 0x21 instead.
    change_rom_bank(&mem, 0x20);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x20));

    // Enable RAM.
    mem.write(0x1010, 0xfa);

    // Then test the RAM.
    change_ram_bank(&mem, 0x00);
    testing::fill_external_ram(&mem, 0x40);
    REQUIRE(testing::verify_external_ram_value(mem, 0x40));
}

TEST_CASE("RAM Banking mode", "[memory][mbc1]") {
    // Generate a ROM with 8 banks, bank 0x00 filled with 0x01, bank 0x01
    // filled with 0x02, bank 0x07 filled with 0x07.
    std::vector<MemoryValue> rom = testing::generate_test_rom(
        8, {{0x00, 0x01}, {0x01, 0x02}, {0x07, 0x07}});

    // Create the MBC and load in the ROM.
    // 8 ROM banks, 4 RAM banks.
    MBC1 mem(rom.size(), 4 * RAM_BANK_SIZE);
    mem.load_rom(rom);

    // Change to RAM addressing mode
    mem.write(0x6069, 1);

    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    change_rom_bank(&mem, 0x01);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));

    change_rom_bank(&mem, 0x07);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x07));

    // Enable RAM
    mem.write(0x1010, 0xfa);

    // Then test the RAM.
    change_ram_bank(&mem, 0x00);
    testing::fill_external_ram(&mem, 0x40);
    change_ram_bank(&mem, 0x03);
    testing::fill_external_ram(&mem, 0x41);

    change_ram_bank(&mem, 0x00);
    REQUIRE(testing::verify_external_ram_value(mem, 0x40));
    change_ram_bank(&mem, 0x03);
    REQUIRE(testing::verify_external_ram_value(mem, 0x41));
}

}  // namespace memory
