#include "memory/flat_rom.h"

#include <catch2/catch.hpp>

#include "memory/regions.h"

namespace memory {

TEST_CASE("ROM read + write", "[memory][FlatROM]") {
    FlatROM flat_rom(0);

    SECTION("Bank 0") {
        for (MemoryAddr i = ROM_0_BEGIN; i <= ROM_0_END; ++i) {
            flat_rom.write(i, i % 256);
        }

        for (MemoryAddr i = ROM_0_BEGIN; i <= ROM_0_END; ++i) {
            REQUIRE(flat_rom.read(i) == (i % 256));
        }
    }

    SECTION("Switchable bank") {
        for (MemoryAddr i = ROM_SWITCHABLE_BEGIN; i <= ROM_SWITCHABLE_END;
             ++i) {
            flat_rom.write(i, i % 128);
        }

        for (MemoryAddr i = ROM_SWITCHABLE_BEGIN; i <= ROM_SWITCHABLE_END;
             ++i) {
            REQUIRE(flat_rom.read(i) == (i % 128));
        }
    }
}

TEST_CASE("RAM read + write", "[memory][FlatROM]") {
    const MemorySize size = 0x100;

    FlatROM flat_rom(size);

    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_BEGIN + size;
         ++i) {
        flat_rom.write(i, i % 256);
    }

    for (MemoryAddr i = RAM_EXTERNAL_BEGIN; i < RAM_EXTERNAL_BEGIN + size;
         ++i) {
        REQUIRE(flat_rom.read(i) == (i % 256));
    }
}

}  // namespace memory
