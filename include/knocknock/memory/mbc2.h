#pragma once

#include "knocknock/memory/memory.h"

namespace memory {

class MBC2 : public ROMLoadableMemory {
public:
    MBC2(std::vector<MemoryValue> rom);

    // Memory::
    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

private:
    static constexpr MemorySize ROM_BANK_SIZE = 0x4000;

    /**
     * Maximum ROM size that the MBC2 can support, the MBC2 supports 16 ROM
     * banks.
     */
    static constexpr MemorySize MAX_ROM_SIZE = 16 * ROM_BANK_SIZE;

    static constexpr MemorySize RAM_SIZE = 0x200;

    bool ram_enabled_;

    uint8_t selected_bank_;

    const std::vector<MemoryValue> rom_;

    // Fixed 512 byte RAM.
    MemoryValue ram_[RAM_SIZE];
};

}  // namespace memory
