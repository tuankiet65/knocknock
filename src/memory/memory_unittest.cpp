/**
 * Test the functionality of read16(), write16() and the MemoryAddrProxy.
 * @file memory_unittest.cpp
 */
#include "memory/memory.h"

#include <catch2/catch.hpp>

#include "memory/test_memory.h"

namespace memory {

TEST_CASE("read16 + write16", "[memory]") {
    TestMemory mem;

    SECTION("8-bit write, 16-bit read") {
        mem.write(0x3456, 0xff);
        mem.write(0x3457, 0x69);
        REQUIRE(mem.read16(0x3456) == 0x69ff);
    }

    SECTION("16-bit write, 8-bit read") {
        mem.write16(0x1726, 0x6938);
        REQUIRE(mem.read(0x1726) == 0x38);
        REQUIRE(mem.read(0x1727) == 0x69);
    }

    SECTION("16-bit write, 16-bit read") {
        mem.write16(0xea8f, 0x1827);
        REQUIRE(mem.read16(0xea8f) == 0x1827);
    }
}

TEST_CASE("MemoryAddrProxy", "[memory]") {
    TestMemory mem1, mem2;

    SECTION("raw write, proxy read") {
        mem1.write(0x7162, 0x17);
        REQUIRE(mem1[0x7162] == 0x17);
    }

    SECTION("proxy write, raw read") {
        mem1[0x11a8] = 0xf8;
        REQUIRE(mem1[0x11a8] == 0xf8);
    }

    SECTION("proxy write, proxy read") {
        mem1[0x8187] = 0x91;
        REQUIRE(mem1[0x8187] == 0x91);
    }

    SECTION("assigning proxies of different memories") {
        mem1[0x1111] = 0x11;
        mem2[0x2222] = 0x22;
        REQUIRE(mem1[0x1111] == 0x11);
        REQUIRE(mem2[0x2222] == 0x22);

        mem1[0x3333] = *mem2[0x2222];
        REQUIRE(mem1[0x3333] == 0x22);
    }
}

}  // namespace memory
