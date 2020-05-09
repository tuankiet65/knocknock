#include <catch2/catch.hpp>

#include "memory/mbc1.h"
#include "memory/regions.h"

namespace {

constexpr memory::MemorySize ROM_BANK_SIZE = 0x4000;
constexpr memory::MemorySize RAM_BANK_SIZE = 0x2000;

}  // namespace

void fill_rom_bank(std::vector<memory::MemoryValue> *rom,
                   uint8_t bank,
                   memory::MemoryValue value) {
    uint32_t start_addr = (uint32_t)(bank)*ROM_BANK_SIZE;

    std::fill(rom->begin() + start_addr,
              rom->begin() + start_addr + ROM_BANK_SIZE, value);
}

void check_rom_bank_value(memory::MBC1 *mem, uint8_t bank, uint8_t value) {
    if (bank == 0) {
        for (auto i = memory::ROM_0_BEGIN; i < memory::ROM_0_END; ++i) {
            REQUIRE((*mem)[i] == value);
        }
    } else {
        // Change the ROM bank to the specified bank.
        // First five bits of the bank number
        mem->write(0x2420, bank & 0b00011111);
        // Bit 6-5 of the bank number
        mem->write(0x4001, (bank >> 5) & 0b11);

        for (auto i = memory::ROM_SWITCHABLE_BEGIN;
             i < memory::ROM_SWITCHABLE_END; ++i) {
            REQUIRE((*mem)[i] == value);
        }
    }
}

void change_ram_bank(memory::MBC1 *mem, uint8_t bank) {
    mem->write(0x4001, bank & 0b11);
}

void fill_ram_bank(memory::MBC1 *mem, uint8_t bank, uint8_t value) {
    change_ram_bank(mem, bank);
    for (auto i = memory::RAM_EXTERNAL_BEGIN; i < memory::RAM_EXTERNAL_END;
         ++i) {
        (*mem)[i] = value;
    }
}

void check_ram_bank_value(memory::MBC1 *mem, uint8_t bank, uint8_t value) {
    change_ram_bank(mem, bank);
    for (auto i = memory::RAM_EXTERNAL_BEGIN; i < memory::RAM_EXTERNAL_END;
         ++i) {
        REQUIRE((*mem)[i] == value);
    }
}

TEST_CASE("ROM Banking mode", "[memory][mbc1]") {
    // Uses a 64 bank ROM.
    const memory::MemorySize rom_size = 64 * ROM_BANK_SIZE;

    // First we generate the ROM.
    std::vector<memory::MemoryValue> rom(rom_size);
    fill_rom_bank(&rom, 0x00, 0x01);
    fill_rom_bank(&rom, 0x01, 0x02);
    fill_rom_bank(&rom, 0x10, 0x10);
    fill_rom_bank(&rom, 0x21, 0x20);

    // Then we create the MBC and load the generated ROM in.
    // 64 ROM banks, 1 RAM bank.
    memory::MBC1 mem(rom_size, RAM_BANK_SIZE);
    mem.load_rom(rom);

    // Change to ROM addressing mode.
    mem.write(0x6069, 0);

    check_rom_bank_value(&mem, 0x00, 0x01);
    check_rom_bank_value(&mem, 0x01, 0x02);
    check_rom_bank_value(&mem, 0x10, 0x10);
    // We previously fill bank 0x21 to 0x20. When we attempt to read from
    // bank 0x20, it should read from bank 0x21 instead.
    check_rom_bank_value(&mem, 0x20, 0x20);

    // Enable RAM.
    mem.write(0x1010, 0xfa);

    // Then test the RAM.
    fill_ram_bank(&mem, 0x00, 0x40);
    check_ram_bank_value(&mem, 0x00, 0x40);
}

TEST_CASE("RAM Banking mode", "[memory][mbc1]") {
    // Uses a 8 bank ROM.
    const memory::MemorySize rom_size = 8 * ROM_BANK_SIZE;

    // Generate the ROM.
    std::vector<memory::MemoryValue> rom(rom_size);
    fill_rom_bank(&rom, 0x00, 0x01);
    fill_rom_bank(&rom, 0x01, 0x02);
    fill_rom_bank(&rom, 0x07, 0x07);

    // Create the MBC and load in the ROM.
    // 8 ROM banks, 4 RAM banks.
    memory::MBC1 mem(rom_size, 4 * RAM_BANK_SIZE);
    mem.load_rom(rom);

    // Change to RAM addressing mode
    mem.write(0x6069, 1);

    check_rom_bank_value(&mem, 0x00, 0x01);
    check_rom_bank_value(&mem, 0x01, 0x02);
    check_rom_bank_value(&mem, 0x07, 0x07);

    // Enable RAM
    mem.write(0x1010, 0xfa);

    // Then test the RAM.
    fill_ram_bank(&mem, 0x00, 0x40);
    fill_ram_bank(&mem, 0x03, 0x41);

    check_ram_bank_value(&mem, 0x00, 0x40);
    check_ram_bank_value(&mem, 0x03, 0x41);
}
