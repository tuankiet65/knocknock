#include "knocknock/memory/flat_rom.h"

#include <fmt/format.h>
#include <glog/logging.h>

namespace memory {

FlatROM::FlatROM(MemorySize ram_size)
    : ram_size_(ram_size),
      ram_end_addr_(RAM_EXTERNAL_BEGIN + ram_size_ - 1),
      rom_(),
      ram_() {
    DCHECK(ram_size_ <= sizeof(ram_));
}

MemoryValue FlatROM::read(MemoryAddr addr) const {
    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END) ||
        BETWEEN(ROM_SWITCHABLE_BEGIN, addr, ROM_SWITCHABLE_END)) {
        return rom_[addr];
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, ram_end_addr_)) {
        return ram_[addr - RAM_EXTERNAL_BEGIN];
    }

    DCHECK(false) << fmt::format("Invalid read at {:#04x}", addr);
    return 0xff;
}

void FlatROM::write(MemoryAddr addr, MemoryValue value) {
    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END) ||
        BETWEEN(ROM_SWITCHABLE_BEGIN, addr, ROM_SWITCHABLE_END)) {
        LOG(ERROR) << fmt::format(
            FMT_STRING("Invalid write to ROM at {:#04x}, ignoring"), addr);
        return;
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, ram_end_addr_)) {
        ram_[addr - RAM_EXTERNAL_BEGIN] = value;
        return;
    }

    DCHECK(false) << fmt::format("Invalid write at {:#04x}", addr);
}

bool FlatROM::load_rom(const std::vector<MemoryValue> &rom) {
    if (rom.size() > sizeof(this->rom_)) {
        return false;
    }

    std::copy(rom.begin(), rom.end(), rom_);
    return true;
}

}  // namespace memory
