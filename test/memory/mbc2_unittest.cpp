#include <catch2/catch.hpp>

#include <knocknock/memory/mbc2.h>
#include <knocknock/memory/regions.h>

#include "memory/unittest_utils.h"

namespace memory {

namespace {

constexpr memory::MemorySize RAM_SIZE = 0x200;  // 512 bytes.

void switch_rom_bank(MBC2 *mem, uint8_t bank) {
    mem->write(0x2101, bank);
}

}  // namespace

TEST_CASE("Default", "[memory][mbc2]") {
    auto rom = testing::generate_test_rom(2, {{0x00, 0x01}, {0x01, 0x02}});

    MBC2 mem(rom);

    REQUIRE(testing::verify_rom_0_value(mem, 0x01));
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));
}

TEST_CASE("ROM", "[memory][mbc2]") {
    auto rom = testing::generate_test_rom(
        16, {{0x00, 0x01}, {0x01, 0x02}, {0x08, 0x08}, {0x0f, 0x0f}});

    MBC2 mem(rom);

    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    switch_rom_bank(&mem, 0x08);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x08));

    switch_rom_bank(&mem, 0x0f);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x0f));

    switch_rom_bank(&mem, 0x00);
    // We expect the MBC to switch to bank 0x01 instead.
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));
}

TEST_CASE("External RAM", "[memory][mbc2]") {
    memory::MBC2 mem({});

    // Enable RAM.
    mem.write(0x0038, 0x0a);

    // Fill the RAM with one constant.
    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_BEGIN + RAM_SIZE;
         ++i) {
        mem[i] = 0x29;
    }

    // The region 0xa200 - 0xa3ff and any subsequent 512-byte blocks mirrors the
    // original RAM at 0xa000 - 0xa1ff, so we'd expect that all bytes in this
    // region reads the same.
    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_END; ++i) {
        // The external RAM is only 4 bit, so when written into, the upper
        // nibble is discarded. As such, here we only check if the lower nibble
        // matches what we wrote into earlier.
        REQUIRE((mem[i] & 0x0f) == 0x09);
    }
}

TEST_CASE("ROM: Out-of-bound read", "[memory][mbc2]") {
    auto rom = testing::generate_test_rom(4, {{0x00, 0x01}, {0x01, 0x02}});

    MBC2 mem(rom);

    REQUIRE(testing::verify_rom_0_value(mem, 0x01));

    switch_rom_bank(&mem, 0x01);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));

    // We switch to bank 0x04, however since the ROM only contains 4 banks,
    // reading from it will return values from bank 0x04 instead.
    switch_rom_bank(&mem, 0x04);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x01));

    // Ditto with bank 0x05, reads will return values from bank 0x01.
    switch_rom_bank(&mem, 0x05);
    REQUIRE(testing::verify_rom_switchable_value(mem, 0x02));
}

}  // namespace memory
