#pragma once

#include "knocknock/memory/memory.h"
#include "knocknock/memory/regions.h"

namespace memory {

/**
 * Flat ROM, with no memory controller and no banks. Optional external RAM
 * can be included. Represents these cartridge types:
 *  * 0x00: ROM only
 *  * 0x08: ROM only + RAM
 *  * 0x09: ROM only + RAM + BATT
 */
class FlatROM : public ROMLoadableMemory {
public:
    /**
     * Initialize a Flat ROM with optional external RAM.
     * @param ram_size Size of the external RAM, or 0 if not available.
     */
    FlatROM(MemorySize ram_size);

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;
    bool load_rom(const std::vector<MemoryValue> &rom) override;

private:
    const MemorySize ram_size_;
    const MemoryAddr ram_end_addr_;

    MemoryValue rom_[ROM_0_SIZE + ROM_SWITCHABLE_SIZE];
    MemoryValue ram_[RAM_EXTERNAL_SIZE];
};

}  // namespace memory
