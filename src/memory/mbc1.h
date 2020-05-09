/**
 * Implementation of the Memory Bank Controller 1.
 * @file mbc1.h
 */
#pragma once

#include "memory/memory.h"

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
         * In this mode, all 7 bits in the bank selection register is used to
         * address the switchable ROM bank. As such, the switchable RAM block
         * points to RAM bank 0. Because the RAM bank can't be changed, the
         * effective external RAM size is 8Kbyte
         */
        ROM_BANKING,
        /**
         * In this mode, the lower 5 bits in the bank selection register is used
         * to address the switchable ROM bank, while the upper 3 bits is used
         * to address the switchable RAM bank. Because the RAM bank can be
         * changed, the effective external RAM size is 32kByte. However,
         * since only 5 bits is allocated for ROM bank selection, only
         * ROM bank xx-xx can be addressed.
         */
        RAM_BANKING
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

    static const MemorySize ROM_BANK_SIZE = 0x4000;
    static const MemorySize RAM_BANK_SIZE = 0x2000;

    /**
     * ROM region. There are a maximum of 128 ROM banks, which resolves to
     * the maximum size of 2048kByte.
     */
    MemoryValue rom_[128 * ROM_BANK_SIZE];
    MemorySize rom_size_;

    /**
     * RAM region. There are a maximum of 4 RAM banks, which resolves to
     * the maximum size of 512kByte.
     */
    MemoryValue ram_[4 * RAM_BANK_SIZE];
    MemorySize ram_size_;
};

}  // namespace memory
