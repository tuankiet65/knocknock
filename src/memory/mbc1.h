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
    MBC1(MemorySize rom_size, MemorySize ram_size);

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;
    bool bulk_load(MemoryAddr addr, MemoryValue data[], size_t len) override;

private:
    enum AddressingMode {
        // ROM_0 is mapped to ROM bank 0, ROM_SWITCHABLE is mapped to ROM bank
        // 0b0{bank2_:2}{bank_1:5}, RAM_SWITCHABLE is mapped to RAM bank 0
        // Effective address space: 1 RAM bank * 8kB/RAM bank = 8kB of RAM
        RAM_8K,
        // ROM_0 is mapped to ROM bank 0b0{bank2_:2}00000, ROM_SWITCHABLE is
        // mapped to ROM bank 0b0{bank2_:2}{bank_1:5}, RAM_SWITCHABLE is mapped
        // to RAM bank 0b000000{bank2_:2}
        // Effective address space: 4 RAM bank * 8kB/RAM bank = 32kB of RAM
        RAM_32K
    };

    MemoryValue get_rom(MemoryAddr bank, MemoryAddr addr) const;
    MemoryValue get_ram(MemoryAddr bank, MemoryAddr addr) const;

    static const MemorySize ROM_BANK_SIZE = 0x4000;
    static const MemorySize RAM_BANK_SIZE = 0x2000;

    // There're a maximm of 128 ROM banks (2 bit bank2_ + 5 bit bank1_)
    // 128 ROM banks * 16kB per bank = 2048kB of ROM
    MemoryValue rom_[128 * ROM_BANK_SIZE];
    MemorySize rom_size_;
    // There're a maximum of 4 RAM banks if mode_ == RAM_32K (2 bit bank2_)
    // 4 RAM banks * 8kB per bank = 32kB of RAM
    MemoryValue ram_[4 * RAM_BANK_SIZE];
    MemorySize ram_size_;

    AddressingMode mode_ = RAM_8K;
    uint8_t bank1_ = 0;
    uint8_t bank2_ = 0;
    bool ram_enabled_ = false;
};

}  // namespace memory
