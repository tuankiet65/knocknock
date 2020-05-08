#include "memory/mbc1.h"

#include <glog/logging.h>

#include <cstring>

namespace memory {

MBC1::MBC1(MemorySize rom_size, MemorySize ram_size)
    : rom_size_(rom_size), ram_size_(ram_size) {
    // DCHECK(rom_size <= sizeof(rom_)) << "ROM size too large";
    // DCHECK(ram_size <= sizeof(ram_)) << "RAM size too large";
}

bool MBC1::load_rom(const std::vector<MemoryValue> &rom) {
    if (rom.size() > rom_size_) {
        return false;
    }

    std::copy(rom.begin(), rom.end(), rom_);
    return true;
}

void MBC1::write(MemoryAddr addr, MemoryValue value) {
    // Enable / disable RAM
    // Command: XXXXSSSS
    //   S: Don't care
    //   B: 1010 (0xA) to enable the RAM, any other value disable the RAM
    if (BETWEEN(0x0000, addr, 0x1fff)) {
        ram_enabled_ = ((value & 0b11110000) == 0xb1010);
        return;
    }

    // BANK1 register
    if (BETWEEN(0x2000, addr, 0x3fff)) {
        bank1_ = value & 0b00011111;
        if (bank1_ == 0)
            bank1_ = 1;
        return;
    }

    // BANK2 register
    if (BETWEEN(0x4000, addr, 0x5fff)) {
        bank2_ = value & 0b00000011;
        return;
    }

    // Set memory mode command
    // Command: XXXXXXXS
    //   S: Don't care
    //   X: 0 for ROM_2MB_RAM_8KB, 1 for ROM_512KB_RAM_32KB
    if (BETWEEN(0x6000, addr, 0x7fff)) {
        if (value & 1)
            mode_ = RAM_32K;
        else
            mode_ = RAM_8K;

        return;
    }

    LOG(ERROR) << "Out of range write to MBC1: 0x" << std::hex << addr;
}

MemoryValue MBC1::get_rom(MemoryAddr bank, MemoryAddr addr) const {
    MemoryAddr real_addr = bank * ROM_BANK_SIZE + addr;
    // DCHECK(real_addr < rom_size_);
    return rom_[bank * 0x4000 + addr];
}

MemoryValue MBC1::get_ram(MemoryAddr bank, MemoryAddr addr) const {
    MemoryAddr real_addr = bank * RAM_BANK_SIZE + addr;
    // DCHECK(real_addr < ram_size_);
    return ram_[bank * 0x2000 + addr];
}

MemoryValue MBC1::read(MemoryAddr addr) const {
    if (BETWEEN(0x0000, addr, 0x3fff)) {
        if (mode_ == RAM_8K)
            return get_rom(0, addr);
        return get_rom(bank2_ << 5, addr);
    }

    if (BETWEEN(0x4000, addr, 0x7fff))
        return get_rom((bank2_ << 5 | bank1_), (addr - 0x4000));

    if (BETWEEN(0xa000, addr, 0xbfff)) {
        if (mode_ == RAM_8K)
            return get_ram(0, (addr - 0xa000));
        return get_ram(bank2_, addr);
    }

    LOG(ERROR) << "Out of range read to MBC1: 0x" << std::hex << addr;
    return 0xff;
}

}  // namespace memory
