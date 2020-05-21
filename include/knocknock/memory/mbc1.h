/**
 * Implementation of the Memory Bank Controller 1.
 * @file mbc1.h
 */
#pragma once

#include "knocknock/memory/memory.h"

namespace memory {

/**
 * The Memory Bank Controller 1, which features up to 2MByte of ROM and/or
 * 32KByte of RAM.
 *
 * These cartridge types corresponds to MBC1:
 * * 0x01 (ROM + MBC1)
 * * 0x02 (ROM + MBC1 + RAM)
 * * 0x05 (ROM + MBC1 + RAM + BATT)
 */
class MBC1 : public ROMLoadableMemory {
public:
    /**
     * Create a MBC1.
     * @param rom_size ROM size.
     * @param ram_size RAM size.
     */
    MBC1(MemorySize rom_size, MemorySize ram_size);

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

    // ROMLoadableMemory::
    bool load_rom(const std::vector<MemoryValue> &rom) override;

private:
    /**
     * Configure how the bank selection register should be understood
     */
    enum class AddressingMode {
        /**
         * In this mode, the ROM_0 region always points to ROM bank 0, and the
         * external RAM region always points to RAM bank 0.
         */
        MODE_0,
        /**
         * In this mode, the ROM_0 region points to ROM bank (bank2_ << 5),
         * allowing access to bank 0x20, 0x40 and 0x60 which is inaccessible
         * in Mode 0. The external RAM region points to RAM bank (bank2_).
         */
        MODE_1
    };

    uint32_t translate_ram_address(MemoryAddr addr) const;

    uint32_t translate_rom_address(MemoryAddr addr) const;

    /**
     * Whether the RAM is enabled or not.
     */
    bool ram_enabled_;

    /**
     * Bank selection register.
     */
    uint8_t bank1_;
    uint8_t bank2_;

    AddressingMode mode_;

    static constexpr MemorySize ROM_BANK_SIZE = 0x4000;
    static constexpr MemorySize RAM_BANK_SIZE = 0x2000;

    /**
     * ROM region. There are a maximum of 128 ROM banks, which resolves to
     * the maximum size of 2MByte.
     */
    MemoryValue rom_[128 * ROM_BANK_SIZE];
    MemorySize rom_size_;

    /**
     * RAM region. There are a maximum of 4 RAM banks, which resolves to
     * the maximum size of 32KByte.
     */
    MemoryValue ram_[4 * RAM_BANK_SIZE];
    MemorySize ram_size_;
};

}  // namespace memory
