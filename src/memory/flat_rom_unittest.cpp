#include "memory/flat_rom.h"

#include <catch2/catch.hpp>

#include "memory/regions.h"

TEST_CASE("ROM read + write", "[memory][FlatROM]") {
    memory::FlatROM flat_rom(0);

    SECTION("Bank 0") {
        for (memory::MemoryAddr i = memory::ROM_0_BEGIN; i <= memory::ROM_0_END;
             ++i) {
            flat_rom.write(i, i % 256);
        }

        for (memory::MemoryAddr i = memory::ROM_0_BEGIN; i <= memory::ROM_0_END;
             ++i) {
            REQUIRE(flat_rom.read(i) == (i % 256));
        }
    }

    SECTION("Switchable bank") {
        for (memory::MemoryAddr i = memory::ROM_SWITCHABLE_BEGIN;
             i <= memory::ROM_SWITCHABLE_END; ++i) {
            flat_rom.write(i, i % 128);
        }

        for (memory::MemoryAddr i = memory::ROM_SWITCHABLE_BEGIN;
             i <= memory::ROM_SWITCHABLE_END; ++i) {
            REQUIRE(flat_rom.read(i) == (i % 128));
        }
    }
}

TEST_CASE("RAM read + write", "[memory][FlatROM]") {
    const memory::MemorySize size = 0x100;

    memory::FlatROM flat_rom(size);

    for (memory::MemoryAddr i = memory::RAM_EXTERNAL_BEGIN;
         i < memory::RAM_EXTERNAL_BEGIN + size; ++i) {
        flat_rom.write(i, i % 256);
    }

    for (memory::MemoryAddr i = memory::RAM_EXTERNAL_BEGIN;
         i < memory::RAM_EXTERNAL_BEGIN + size; ++i) {
        REQUIRE(flat_rom.read(i) == (i % 256));
    }
}