#pragma once

#include "memory/memory.h"

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
    // ram_size_ needs to be specified in case the cartridge sports external
    // RAM (which is mapped to the cartridge RAM region)
    FlatROM(MemorySize ram_size);

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;
    bool load_rom(const std::vector<MemoryValue> &rom) override;

private:
    MemorySize ram_size_;

    MemoryValue rom_[0x8fff];
    MemoryValue ram_[0x2000];
};

}  // namespace memory
