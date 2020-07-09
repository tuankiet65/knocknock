#include <catch2/catch.hpp>

#include <knocknock/memory/flat_rom.h>

#include "memory/unittest_utils.h"

namespace memory {

TEST_CASE("ROM read", "[memory][FlatROM]") {
    auto rom = testing::generate_test_rom(2, {{0x00, 0x01}, {0x01, 0x02}});

    FlatROM flat_rom(rom, 0);

    REQUIRE(testing::verify_rom_0_value(flat_rom, 0x01));
    REQUIRE(testing::verify_rom_switchable_value(flat_rom, 0x02));
}

TEST_CASE("RAM read + write", "[memory][FlatROM]") {
    const MemorySize ram_size = 0x2000;  // the size of the external RAM area.

    FlatROM flat_rom({}, ram_size);

    testing::fill_external_ram(&flat_rom, 0x8a);
    REQUIRE(testing::verify_external_ram_value(flat_rom, 0x8a));
}

}  // namespace memory
