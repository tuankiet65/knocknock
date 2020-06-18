#include "knocknock/memory/mbc1.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include "knocknock/memory/regions.h"

namespace memory {

namespace {

constexpr int RAM_ENABLE_BEGIN = 0x0000;
constexpr int RAM_ENABLE_END = 0x1fff;

constexpr int BANK1_BEGIN = 0x2000;
constexpr int BANK1_END = 0x3fff;

constexpr int BANK2_BEGIN = 0x4000;
constexpr int BANK2_END = 0x5fff;

constexpr int ADDR_MODE_BEGIN = 0x6000;
constexpr int ADDR_MODE_END = 0x7fff;

}  // namespace

MBC1::MBC1(MemorySize rom_size, MemorySize ram_size)
    : ram_enabled_(false),
      bank1_(1),
      bank2_(0),
      mode_(AddressingMode::MODE_0),
      rom_(),
      rom_size_(rom_size),
      ram_(),
      ram_size_(ram_size) {
    DCHECK(rom_size_ <= sizeof(rom_))
        << fmt::format("ROM size too large: {}", rom_size_);
    DCHECK(ram_size_ <= sizeof(ram_))
        << fmt::format("RAM size too large: {}", ram_size_);
}

bool MBC1::load_rom(const std::vector<MemoryValue> &rom) {
    if (rom.size() > rom_size_) {
        return false;
    }

    std::copy(rom.begin(), rom.end(), rom_);
    return true;
}

uint32_t MBC1::translate_rom_address(MemoryAddr addr) const {
    uint32_t bank = 0;
    uint32_t addr_in_bank = 0;

    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END)) {
        // Reading from bank 0.
        switch (mode_) {
            case AddressingMode::MODE_0: bank = 0; break;
            case AddressingMode::MODE_1: bank = bank2_ << 5; break;
        }

        addr_in_bank = addr - ROM_0_BEGIN;
    } else if (BETWEEN(ROM_SWITCHABLE_BEGIN, addr, ROM_SWITCHABLE_END)) {
        // Reading from switchable bank area.
        bank = (bank2_ << 5) | bank1_;
        addr_in_bank = addr - ROM_SWITCHABLE_BEGIN;
    } else {
        DCHECK(false);
    }

    uint32_t real_addr = bank * ROM_BANK_SIZE + addr_in_bank;
    real_addr %= rom_size_;

    return real_addr;
}

uint32_t MBC1::translate_ram_address(MemoryAddr addr) const {
    uint32_t bank = 0;
    uint32_t addr_in_bank = addr - RAM_EXTERNAL_BEGIN;

    if (mode_ == AddressingMode::MODE_0) {
        bank = 0;
    } else if (mode_ == AddressingMode::MODE_1) {
        bank = bank2_;
    }

    uint32_t real_addr = bank * RAM_BANK_SIZE + addr_in_bank;
    real_addr %= ram_size_;

    return real_addr;
}

MemoryValue MBC1::read(MemoryAddr addr) const {
    if (BETWEEN(ROM_0_BEGIN, addr, ROM_0_END) ||
        BETWEEN(ROM_SWITCHABLE_BEGIN, addr, ROM_SWITCHABLE_END)) {
        uint32_t real_addr = translate_rom_address(addr);

        return rom_[real_addr];
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, RAM_EXTERNAL_END)) {
        if (!ram_enabled_) {
            LOG(INFO) << "RAM not enabled, returning junk value";
            return 0xff;
        }

        uint32_t real_addr = translate_ram_address(addr);

        return ram_[real_addr];
    }

    LOG(ERROR) << fmt::format(
        "Out of range read to MBC1: {:#04x}, returning junk value", addr);
    return 0xff;
}

void MBC1::write(MemoryAddr addr, MemoryValue value) {
    // Enable / disable RAM
    // Command: XXXXSSSS
    //   X: Don't care
    //   S: 1010 (0xA) to enable the RAM, any other value to disable it.
    if (BETWEEN(RAM_ENABLE_BEGIN, addr, RAM_ENABLE_END)) {
        ram_enabled_ = (value & 0xAu);
        return;
    }

    // BANK1 register. Only the lower 5 bits count.
    if (BETWEEN(BANK1_BEGIN, addr, BANK1_END)) {
        bank1_ = value & 0b00011111u;
        if (bank1_ == 0)
            bank1_ = 1;
        return;
    }

    // BANK2 register. Only the lower 2 bits count.
    if (BETWEEN(BANK2_BEGIN, addr, BANK2_END)) {
        bank2_ = value & 0b00000011u;
        return;
    }

    // Set addressing mode.
    // Command: XXXXXXXS
    //   X: Don't care
    //   S: 0 for ROM banking mode, 1 for RAM banking mode.
    if (BETWEEN(ADDR_MODE_BEGIN, addr, ADDR_MODE_END)) {
        if ((value & 1u) == 0) {
            mode_ = AddressingMode::MODE_0;
        } else {
            mode_ = AddressingMode::MODE_1;
        }

        return;
    }

    if (BETWEEN(RAM_EXTERNAL_BEGIN, addr, RAM_EXTERNAL_END)) {
        if (!ram_enabled_) {
            LOG(ERROR) << fmt::format(
                "RAM not enabled, ignoring write to addr {:#04x}", addr);
            return;
        }

        uint32_t real_addr = translate_ram_address(addr);
        if (real_addr >= ram_size_) {
            LOG(ERROR) << "Attempt to read past the RAM area, ignoring";
            return;
        }

        ram_[real_addr] = value;

        return;
    }

    LOG(ERROR) << fmt::format("Out of range write to MBC1: {:#04x}", addr);
}

}  // namespace memory
