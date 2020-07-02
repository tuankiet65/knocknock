#include <catch2/catch.hpp>

#include <knocknock/ppu/dma.h>

#include <knocknock/memory/test_memory.h>

namespace ppu {

namespace {

constexpr memory::MemoryAddr SOURCE_ADDR = 0x4500;
constexpr uint8_t SIZE = 160;
constexpr memory::MemoryAddr DMA_ADDR = 0xff46;
constexpr memory::MemoryAddr OAM_ADDR = 0xfe00;

}  // namespace

TEST_CASE("DMA", "[ppu]") {
    memory::TestMemory memory;

    // Fill the area [SOURCE_ADDR, SOURCE_ADDR + SIZE) with the test pattern,
    // we'll use DMA to copy this area to the OAM.
    for (uint8_t i = 0; i < SIZE; ++i) {
        memory[SOURCE_ADDR + i] = i;
    }

    DMA dma(&memory);
    // Start the DMA process.
    dma[DMA_ADDR] = SOURCE_ADDR / 0x100;

    // Wait for 160 ticks for the process to complete.
    for (uint8_t i = 0; i < SIZE; ++i) {
        dma.tick();
    }

    // Check if the test pattern above is copied to the OAM area.
    for (uint8_t i = 0; i < SIZE; ++i) {
        REQUIRE(memory[OAM_ADDR + i] == i);
    }
}

}  // namespace ppu
