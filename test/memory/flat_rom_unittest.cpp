#include <catch2/catch.hpp>

#include <knocknock/memory/flat_rom.h>

#include "memory/unittest_utils.h"

namespace memory {

TEST_CASE("ROM read", "[memory][FlatROM]") {
    auto rom = testing::generate_test_rom(2, {{0x00, 0x01}, {0x01, 0x02}});

    FlatROM flat_rom(0);
    flat_rom.load_rom(rom);

    REQUIRE(testing::verify_rom_0_value(flat_rom, 0x01));
    REQUIRE(testing::verify_rom_switchable_value(flat_rom, 0x02));
}

TEST_CASE("RAM read + write", "[memory][FlatROM]") {
    const MemorySize ram_size = 0x100;

    FlatROM flat_rom(ram_size);

    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_BEGIN + ram_size;
         ++i) {
        flat_rom.write(i, i % 256);
    }

    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_BEGIN + ram_size;
         ++i) {
        REQUIRE(flat_rom.read(i) == (i % 256));
    }
}

}  // namespace memory
