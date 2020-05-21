#pragma once

#include "knocknock/memory/memory.h"

namespace memory {

class MBC2 : public ROMLoadableMemory {
public:
    MBC2(MemorySize rom_size);

    // Memory::
    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

    // ROMLoadableMemory::
    bool load_rom(const std::vector<MemoryValue> &rom) override;

private:
    static constexpr MemorySize ROM_BANK_SIZE = 0x4000;
    static constexpr MemorySize RAM_SIZE = 0x200;

    bool ram_enabled_;

    uint8_t selected_bank_;

    // Maximum of 16 ROM banks.
    MemoryValue rom_[16 * ROM_BANK_SIZE];
    MemorySize rom_size_;

    // Fixed 512 byte RAM.
    MemoryValue ram_[RAM_SIZE];
};

}  // namespace memory
