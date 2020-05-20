#include <catch2/catch.hpp>

#include "memory/internal_ram.h"

#include "memory/regions.h"

namespace memory {

TEST_CASE("Internal RAM", "[memory]") {
    InternalRAM ram;

    for (auto i = RAM_INTERNAL_BEGIN; i <= RAM_INTERNAL_END; ++i) {
        ram[i] = 0x92;
    }

    for (auto i = RAM_INTERNAL_BEGIN; i <= RAM_INTERNAL_END; ++i) {
        REQUIRE(ram[i] == 0x92);
    }
}

TEST_CASE("Echo RAM", "[memory]") {
    InternalRAM ram;

    // Write to internal RAM and read from echo RAM
    for (auto i = RAM_INTERNAL_BEGIN; i <= RAM_INTERNAL_END; ++i) {
        ram[i] = 0x39;
    }

    for (auto i = RAM_ECHO_BEGIN; i <= RAM_ECHO_END; ++i) {
        REQUIRE(ram[i] == 0x39);
    }

    // Write to echo RAM and read from internal RAM.
    for (auto i = RAM_ECHO_BEGIN; i <= RAM_ECHO_END; ++i) {
        ram[i] = 0x2f;
    }

    // Echo RAM is smaller than internal RAM so we only check the
    // overlapping region.
    for (auto i = RAM_INTERNAL_BEGIN; i < RAM_INTERNAL_BEGIN + RAM_ECHO_SIZE;
         ++i) {
        REQUIRE(ram[i] == 0x2f);
    }

    // Check that the part not mirrored to the echo RAM region still stays the
    // same value
    for (auto i = RAM_INTERNAL_BEGIN + RAM_ECHO_SIZE; i <= RAM_INTERNAL_END;
         ++i) {
        REQUIRE(ram[i] == 0x39);
    }
}

TEST_CASE("HRAM", "[memory]") {
    InternalRAM ram;

    for (auto i = HRAM_BEGIN; i <= HRAM_END; ++i) {
        ram[i] = 0x67;
    }

    for (auto i = HRAM_BEGIN; i <= HRAM_END; ++i) {
        REQUIRE(ram[i] == 0x67);
    }
}

}  // namespace memory
