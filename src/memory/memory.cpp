#include "memory/memory.h"

#include <stdexcept>

namespace memory {

namespace {

// TODO: correct value
const uint16_t ROM_SWITCHABLE_OFFSET = 0xffff;
const uint16_t RAM_ECHO_OFFSET = 0xff;

} // namespace

bool Memory::get_region_and_offset(uint16_t addr,
                                   MemoryRegion *region,
                                   uint16_t *offset) const {
    // TODO: DCHECK(region)
    // TODO: DCHECK(offset)

    if (BETWEEN(0x0000, addr, 0x3fff)) {
        *region = ROM_0;
        *offset = 0x0000;
        return true;
    }

    if (BETWEEN(0x4000, addr, 0x7fff)) {
        *region = ROM_SWITCHABLE;
        *offset = 0x4000;
        return true;
    }

    if (BETWEEN(0x8000, addr, 0x9fff)) {
        *region = RAM_VIDEO;
        *offset = 0x8000;
        return true;
    }

    if (BETWEEN(0xa000, addr, 0xbfff)) {
        *region = RAM_SWITCHABLE;
        *offset = 0xa000;
        return true;
    }

    if (BETWEEN(0xc000, addr, 0xdfff)) {
        *region = RAM;
        *offset = 0xc000;
        return true;
    }

    if (BETWEEN(0xe000, addr, 0xfdff)) {
        *region = RAM_ECHO;
        *offset = 0xe000;
        return true;
    }

    if (BETWEEN(0xfe00, addr, 0xfe9f)) {
        *region = OAM;
        *offset = 0xfe00;
        return true;
    }

    if (BETWEEN(0xff00, addr, 0xff7f)) {
        *region = IO;
        *offset = 0xff00;
        return true;
    }

    if (BETWEEN(0xff80, addr, 0xfffe)) {
        *region = RAM_HIGH;
        *offset = 0xff80;
        return true;
    }

    if (BETWEEN(0xffff, addr, 0xffff)) {
        *region = INTERRUPT_REG;
        *offset = 0xffff;
        return true;
    }

    return false;
}

bool Memory::get(uint16_t addr, uint8_t *result) const {
    MemoryRegion region;
    uint16_t offset;

    if (!get_region_and_offset(addr, &region, &offset))
        return false;

    addr -= offset;
    switch (region) {
        case ROM_0: *result = rom_0_[addr]; break;
        // TODO: implement ROM bank
        case ROM_SWITCHABLE: *result = rom_0_[addr - ROM_SWITCHABLE_OFFSET]; break;
        case RAM_VIDEO: *result = ram_video_[addr]; break;
        case RAM: *result = ram_[addr]; break;
        case RAM_ECHO: *result = ram_[addr - RAM_ECHO_OFFSET]; break;
        case OAM: *result = oam_[addr]; break;
        case IO: *result = io_[addr]; break;
        case RAM_HIGH: *result = ram_high_[addr]; break;
        case INTERRUPT_REG: *result = interrupt_reg_; break;
        default: /* TODO: NOTREACHED() */ return false;
    }

    return true;
}

bool Memory::set(uint16_t addr, uint8_t value) {
    MemoryRegion region;
    uint16_t offset;

    if (!get_region_and_offset(addr, &region, &offset))
        return false;

    addr -= offset;
    switch (region) {
        case ROM_0: rom_0_[addr] = value; break;
        // TODO: implement ROM bank
        case ROM_SWITCHABLE: rom_0_[addr - ROM_SWITCHABLE_OFFSET] = value; break;
        case RAM_VIDEO: ram_video_[addr] = value; break;
        case RAM: ram_[addr] = value; break;
        case RAM_ECHO: ram_[addr - RAM_ECHO_OFFSET] = value; break;
        case OAM: oam_[addr] = value; break;
        case IO: io_[addr] = value; break;
        case RAM_HIGH: ram_high_[addr] = value; break;
        case INTERRUPT_REG: interrupt_reg_ = value; break;
        default: /* TODO: NOTREACHED() */ return false;
    }

    return true;
}

uint8_t Memory::operator[](uint16_t addr) const {
    uint8_t result;
    if (!get(addr, &result))
        throw std::out_of_range("invalid address");
    return result;
}

Memory::AssignmentProxy& Memory::operator[](uint16_t addr) {
}

} // namespace cpu
