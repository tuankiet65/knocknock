#include "memory/flat_rom.h"

#include <glog/logging.h>

#include "memory/memory.h"

using std::hex;

namespace memory {

FlatROM::FlatROM(MemorySize ram_size)
    : ram_size_(ram_size) {
    DCHECK(ram_size_ <= sizeof(ram_));
}

MemoryValue FlatROM::read(MemoryAddr addr) const {
    if (BETWEEN(0x0000, addr, 0x3fff)) {
        return rom_[addr];
    }

    if (BETWEEN(0xa000, addr, 0xbfff)) {
        if ((addr - 0xa000) >= ram_size_) {
            LOG(ERROR) << "Out of range RAM read at addr 0x" << hex << addr
                       << ", returning dummy value";
            return 0xff;
        }

        return ram_[addr];
    }

    DCHECK(false) << "Access to invalid FlatROM range: 0x" << hex << addr;
    return 0xff;
}

} // namespace memory
