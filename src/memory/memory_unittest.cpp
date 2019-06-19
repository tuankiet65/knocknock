#include <catch2/catch.hpp>

#include "memory/memory.h"

TEST_CASE("get", "[memory]") {
    memory::Memory mem;

    uint8_t tmp;
    CHECK(mem.get(0x0000, &tmp));
    CHECK(mem.get(0xffff, &tmp));

    CHECK_FALSE(mem.get(0xfea0, &tmp));
    CHECK_FALSE(mem.get(0xfeaf, &tmp));
}

TEST_CASE("writing to ROM0", "[memory]") {
    memory::Memory mem;

    CHECK_FALSE(mem.set(0x0000, 0xff));
    CHECK_FALSE(mem.set(0x3210, 0xff));
    CHECK_FALSE(mem.set(0x3fff, 0xff));
}
