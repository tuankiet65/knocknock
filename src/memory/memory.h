#pragma once

#include <cstdint>

#define BETWEEN(l, x, r) ((l) <= (x) && (x) <= (r))

namespace memory {

class Memory {
public:
    class AssignmentProxy {
    private:
        uint8_t value;
    };

    bool get(uint16_t addr, uint8_t *value) const;
    bool set(uint16_t addr, uint8_t value);

    uint8_t operator[](uint16_t addr) const;
    // AssignmentProxy& operator[](uint16_t addr);

private:
    uint8_t rom_0_[0x8000]; // 0x0000 to 0x7fff
    uint8_t ram_video_[0x2000]; // 0x8000 to 0x9fff
    uint8_t ram_[0x2000]; // 0xc000 to 0xdfff
    // TODO: correct capacity
    uint8_t oam_[0x1000]; // 0xfe00 to 0xfe9f
    // TODO: correct capacity
    uint8_t io_[0x1000]; // 0xff00 to 0xff4b
    // TODO: correct capacity
    uint8_t ram_high_[0x1000]; // 0xff80 to 0xfffe
    uint8_t interrupt_reg_; // 0xffff

    enum MemoryRegion {
        ROM_0,
        ROM_SWITCHABLE,
        RAM_VIDEO,
        RAM_SWITCHABLE,
        RAM,
        RAM_ECHO,
        OAM,
        IO,
        RAM_HIGH,
        INTERRUPT_REG
    };

    bool get_region_and_offset(uint16_t addr,
                               MemoryRegion *region,
                               uint16_t *offset) const;

};

} // namespace cpu
