#include "knocknock/memory/mbc2.h"

#include <glog/logging.h>

#include "knocknock/memory/regions.h"

namespace memory {

namespace {

constexpr MemoryAddr BANK_REG_ADDR_MASK = 1 << 8;

uint8_t low_nibble(uint8_t value) {
    return value & 0x0Fu;
}

}  // namespace

MBC2::MBC2(MemorySize rom_size)
    : ram_enabled_(false),
      selected_bank_(1),
      rom_(),
      rom_size_(rom_size),
      ram_() {
    DCHECK(rom_size <= sizeof(rom_));
}

MemoryValue MBC2::read(MemoryAddr addr) const {
    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END)) {
        return rom_[addr];
    }

    if (BETWEEN(ROM_SWITCHABLE_BEGIN, addr, ROM_SWITCHABLE_END)) {
        uint32_t real_addr =
            (selected_bank_ * ROM_BANK_SIZE) + (addr - ROM_SWITCHABLE_BEGIN);

        real_addr %= rom_size_;

        return rom_[real_addr];
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, RAM_EXTERNAL_END)) {
        if (!ram_enabled_) {
            LOG(ERROR) << "Reading from disabled RAM, returning dummy value";
            return 0xff;
        }

        MemoryAddr real_addr = (addr - RAM_EXTERNAL_BEGIN) % RAM_SIZE;

        // BGB says to make the upper nibble 0xF.
        // https://bgb.bircd.org/mbc2save.html
        return (0xf0 | ram_[real_addr]);
    }

    LOG(ERROR) << "Invalid read to MBC2, returning dummy value";
    return 0xff;
}

void MBC2::write(MemoryAddr addr, MemoryValue value) {
    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END)) {
        if (addr & BANK_REG_ADDR_MASK) {
            selected_bank_ = low_nibble(value);
            if (selected_bank_ == 0) {
                selected_bank_ = 1;
            }
        } else {
            ram_enabled_ = (value & 0x0a);
        }

        return;
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, RAM_EXTERNAL_END)) {
        if (!ram_enabled_) {
            LOG(ERROR) << "Writing to disabled RAM, ignoring";
            return;
        }

        MemoryAddr real_addr = (addr - RAM_EXTERNAL_BEGIN) % RAM_SIZE;
        ram_[real_addr] = low_nibble(value);
        return;
    }

    LOG(ERROR) << "Unknown write to MBC2, ignoring.";
}

bool MBC2::load_rom(const std::vector<MemoryValue> &rom) {
    if (rom.size() > rom_size_) {
        return false;
    }

    std::copy(rom.begin(), rom.end(), rom_);
    return true;
}

}  // namespace memory
