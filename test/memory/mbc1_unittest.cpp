#include <catch2/catch.hpp>

#include <knocknock/memory/mbc1.h>

#include "memory/unittest_utils.h"

namespace memory {

namespace {

constexpr MemorySize RAM_BANK_SIZE = 0x2000;

void switch_rom_bank(MBC1 *mem, uint8_t bank) {
    // First five bits of the bank number
    mem->write(0x2420, bank & 0b00011111);
    // Bit 6-5 of the bank number
    mem->write(0x4001, (bank >> 5) & 0b11);
}

void switch_ram_bank(MBC1 *mem, uint8_t bank) {
    mem->write(0x4001, bank & 0b11);
}

}  // namespace

TEST_CASE("Initial bank in switchable region", "[memory][mbc1]") {
    // Generate a ROM with 128 banks, bank 0x00 filled with 0x01, bank 0x01
    // filled with 0x02.
    auto rom = testing::generate_test_rom(0x80, {{0x00, 0x01}, {0x01, 0x02}});

    // Create the MBC.
    MBC1 mem(rom.size(), 0);
    mem.load_rom(rom);

    // Set to mode 0.
    mem.write(0x6069, 0);

    // Verify that in mode 0, ROM_0 region always points to bank 0.
    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    // Verify that the switchable region initially points to bank 1.
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));
}

TEST_CASE("ROM Bank 0 region", "[memory][mbc1]") {
    // Generate a ROM with 128 banks, bank 0x00 filled with 0x01, bank 0x20
    // filled with 0x20.
    auto rom = testing::generate_test_rom(0x80, {{0x00, 0x01}, {0x20, 0x20}});

    // Create the MBC.
    MBC1 mem(rom.size(), 0);
    mem.load_rom(rom);

    // Set to mode 0.
    mem.write(0x6069, 0);

    // Randomly switch the ROM bank to something else.
    switch_rom_bank(&mem, 0x35);

    // In mode 0, ROM_0 region always points to bank 0 regardless of which bank
    // is switched to. Verify that is correct.
    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    // Set to mode 1
    mem.write(0x6069, 1);

    // Then switch to bank 0x25.
    switch_rom_bank(&mem, 0x25);

    // bank2_ should be 0x1, which means that ROM_0 should actually points to
    // bank 0x20.
    REQUIRE(testing::verify_rom_0_value(mem, 0x20));
}

TEST_CASE("ROM Switchable bank region", "[memory][mbc1]") {
    // Generate a ROM with 128 banks, bank 0x24 filled with 0x24, bank 0x41
    // filled with 0x41, bank 0x69 filled with 0x69 (nice).
    auto rom = testing::generate_test_rom(
        0x80, {{0x24, 0x24}, {0x41, 0x41}, {0x69, 0x69}});

    MBC1 mem(rom.size(), 0);
    mem.load_rom(rom);

    // Set to mode 0
    mem.write(0x6069, 0);

    switch_rom_bank(&mem, 0x24);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x24));
    // When bank 0x40 is selected then the switchable bank region should switch
    // to bank 0x41 instead.
    switch_rom_bank(&mem, 0x40);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x41));
    switch_rom_bank(&mem, 0x69);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x69));

    // Set to mode 1
    mem.write(0x6069, 1);

    // Same test as above, since the mode should not affect the switchable bank
    // region.
    switch_rom_bank(&mem, 0x24);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x24));
    // When bank 0x40 is selected then the switchable bank region should switch
    // to bank 0x41 instead.
    switch_rom_bank(&mem, 0x40);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x41));
    switch_rom_bank(&mem, 0x69);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x69));
}

TEST_CASE("External RAM region", "[memory][mbc1]") {
    MBC1 mem(0, 4 * RAM_BANK_SIZE);

    // Enable RAM.
    mem.write(0x1010, 0xfa);

    // Set to mode 0.
    mem.write(0x6069, 0);

    // Fill bank 0 to 0x01.
    // In Mode 0, the external RAM region should always point to RAM bank 0.
    testing::fill_external_ram(&mem, 0x01);

    // Randomly switch to another RAM bank.
    switch_ram_bank(&mem, 0x03);

    // In Mode 0, the external RAM region should always point to RAM bank 0.
    // Verify this.
    REQUIRE(testing::verify_external_ram_value(mem, 0x01));

    // Set to mode 1
    mem.write(0x6069, 1);

    switch_ram_bank(&mem, 0x02);
    testing::fill_external_ram(&mem, 0x02);
    switch_ram_bank(&mem, 0x03);
    testing::fill_external_ram(&mem, 0x03);

    switch_ram_bank(&mem, 0x02);
    REQUIRE(testing::verify_external_ram_value(mem, 0x02));
    switch_ram_bank(&mem, 0x03);
    REQUIRE(testing::verify_external_ram_value(mem, 0x03));
}

TEST_CASE("Out-of-bound ROM read", "[memory][mbc1]") {
    auto rom = testing::generate_test_rom(4, {{0x00, 0x01}, {0x01, 0x02}});

    MBC1 mem(rom.size(), 0);
    mem.load_rom(rom);

    // Set to mode 0
    mem.write(0x6069, 0);

    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    switch_rom_bank(&mem, 0x01);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));

    // We switch to bank 0x04, however since the ROM only contains 4 banks,
    // reading from it will return values from bank 0x00 instead.
    switch_rom_bank(&mem, 0x04);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x01));

    // Ditto with bank 0x05, reads will return values from bank 0x01.
    switch_rom_bank(&mem, 0x05);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));
}

TEST_CASE("Out-of-bound RAM read", "[memory][mbc1]") {
    // Create a RAM-only MBC1 with 2 RAM banks.
    MBC1 mem(0, 2 * RAM_BANK_SIZE);

    // Enable RAM.
    mem.write(0x1010, 0xfa);

    // Set to mode 1 so we can address all RAM banks.
    mem.write(0x6069, 1);

    // Fill the first RAM bank with 0x01 and the second bank with 0x02.
    switch_ram_bank(&mem, 0x00);
    testing::fill_external_ram(&mem, 0x01);
    switch_ram_bank(&mem, 0x01);
    testing::fill_external_ram(&mem, 0x02);

    // Switch to RAM bank 0x02. Since the MBC only contains 2 RAM banks, reading
    // from it will return values from bank 0x00 instead.
    switch_ram_bank(&mem, 0x02);
    REQUIRE(testing::verify_external_ram_value(mem, 0x01));

    // Ditto with RAM bank 0x03, reads will return values from bank 0x01.
    switch_ram_bank(&mem, 0x03);
    REQUIRE(testing::verify_external_ram_value(mem, 0x02));
}

}  // namespace memory
