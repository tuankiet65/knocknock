#include "knocknock/ppu/ppu.h"

#include "knocknock/memory/memory.h"

#include <glog/logging.h>

namespace ppu {

namespace {

constexpr memory::MemoryAddr LCDC = 0xff40;
constexpr memory::MemoryValue LCDC_ENABLE = 1 << 7;
constexpr memory::MemoryValue LCDC_WINDOW_MAP_SELECT = 1 << 6;
constexpr memory::MemoryValue LCDC_WINDOW_ENABLE = 1 << 5;
constexpr memory::MemoryValue LCDC_ADDRESSING_MODE = 1 << 4;
constexpr memory::MemoryValue LCDC_BG_MAP_SELECT = 1 << 3;
constexpr memory::MemoryValue LCDC_SPRITE_SIZE = 1 << 2;
constexpr memory::MemoryValue LCDC_SPRITE_ENABLE = 1 << 1;
constexpr memory::MemoryValue LCDC_BG_WINDOW_PRIORITY = 1 << 0;

constexpr memory::MemoryAddr STAT = 0xff41;
constexpr memory::MemoryValue STAT_COINCIDENCE_INTERRUPT = 1 << 6;
constexpr memory::MemoryValue STAT_OAM_SCAN_INTERRUPT = 1 << 5;
constexpr memory::MemoryValue STAT_VBLANK_INTERRUPT = 1 << 4;
constexpr memory::MemoryValue STAT_HBLANK_INTERRUPT = 1 << 3;
constexpr memory::MemoryValue STAT_COINCIDENCE = 1 << 2;

constexpr memory::MemoryAddr SCY = 0xff42;
constexpr memory::MemoryAddr SCX = 0xff43;
constexpr memory::MemoryAddr LY = 0xff44;
constexpr memory::MemoryAddr LYC = 0xff45;
constexpr memory::MemoryAddr BGP = 0xff47;
constexpr memory::MemoryAddr OBP0 = 0xff48;
constexpr memory::MemoryAddr OBP1 = 0xff49;
constexpr memory::MemoryAddr WY = 0xff4a;
constexpr memory::MemoryAddr WX = 0xff4b;

constexpr uint8_t OAM_BG_PRIORITY = 1 << 7;
constexpr uint8_t OAM_Y_FLIP = 1 << 6;
constexpr uint8_t OAM_X_FLIP = 1 << 5;
constexpr uint8_t OAM_PALETTE = 1 << 4;

constexpr memory::MemoryAddr TILE_DATA_BEGIN = 0x8000;
constexpr memory::MemoryAddr TILE_DATA_END = 0x97ff;

constexpr memory::MemoryAddr BG_MAP_9800_BEGIN = 0x9800;
constexpr memory::MemoryAddr BG_MAP_9800_END = 0x9bff;

constexpr memory::MemoryAddr BG_MAP_9C00_BEGIN = 0x9c00;
constexpr memory::MemoryAddr BG_MAP_9C00_END = 0x9fff;

uint8_t read_palette(const Palette &palette) {
    uint8_t result = 0;

    for (int i = 3; i >= 0; --i) {
        DCHECK((0 <= palette[i]) && (palette[i] < 4));
        result = (result | palette[i]) << 2;
    }

    return result;
}

void write_palette(Palette *palette, uint8_t value) {
    for (int i = 0; i < 4; ++i) {
        (*palette)[i] = value & 0b00000011;
        value >> 2;
    }
}

uint16_t translate_tile_id(uint8_t tile_id, PPU::AddressingMode mode) {
    if (mode == PPU::AddressingMode::MODE_8000) {
        return tile_id;
    }

    return 256 + static_cast<int8_t>(tile_id);
}

}  // namespace

PPU::PPU() : mode_(Mode::OAM_SCAN), ly_(0) {}

void PPU::register_to_mmu(memory::MMU *mmu) {
    mmu->register_region(this, memory::VRAM_BEGIN, memory::VRAM_END);
    mmu->register_region(this, memory::OAM_BEGIN, memory::OAM_END);
    mmu->register_region(this, LCDC, WX);
}

memory::MemoryValue PPU::read_lcdc() const {
    return lcdc_;
}

memory::MemoryValue PPU::read_stat() const {
    // Clear out the bits for LY == LYC and mode flag.
    memory::MemoryValue result = stat_ & 0b11111000;

    // LY == LYC?
    if (ly_ == lyc_) {
        result |= STAT_COINCIDENCE;
    }

    // Mode flag.
    result |= static_cast<memory::MemoryValue>(mode_);

    return result;
}

memory::MemoryValue PPU::read(memory::MemoryAddr addr) const {
    // Individual register.
    switch (addr) {
        case LCDC: return read_lcdc();
        case STAT: return read_stat();
        case SCY: return scy_;
        case SCX: return scx_;
        case LY: return ly_;
        case LYC: return lyc_;
        case BGP: return read_palette(bgp_); break;
        case OBP0: return read_palette(obp0_); break;
        case OBP1: return read_palette(obp1_); break;
        case WY: return wy_;
        case WX: return wx_;
        default: break;
    }

    if (BETWEEN(memory::OAM_BEGIN, addr, memory::OAM_END)) {
        if (mode_ == Mode::HBLANK || mode_ == Mode::VBLANK) {
            return oam_[addr - memory::OAM_BEGIN];
        } else {
            LOG(ERROR) << "Reading from OAM during OAM scan or LCD draw, "
                          "returning dummy";
            return 0xff;
        }
    }

    if (BETWEEN(TILE_DATA_BEGIN, addr, TILE_DATA_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            return tiles_[addr - TILE_DATA_BEGIN];
        } else {
            LOG(ERROR) << "Reading from VRAM during LCD draw, returning dummy.";
            return 0xff;
        }
    }

    if (BETWEEN(BG_MAP_9800_BEGIN, addr, BG_MAP_9800_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            return bg_map_9800_[addr - BG_MAP_9800_BEGIN];
        } else {
            LOG(ERROR) << "Reading from VRAM during LCD draw, returning dummy.";
            return 0xff;
        }
    }

    if (BETWEEN(BG_MAP_9C00_BEGIN, addr, BG_MAP_9C00_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            return bg_map_9c00_[addr - BG_MAP_9C00_BEGIN];
        } else {
            LOG(ERROR) << "Reading from VRAM during LCD draw, returning dummy.";
            return 0xff;
        }
    }

    LOG(ERROR) << "Unknown read to PPU, returning dummy";
    return 0xff;
}

void PPU::write_lcdc(memory::MemoryValue value) {
    lcdc_ = value;

    lcd_enabled_ = lcdc_ & LCDC_ENABLE;
    window_tile_map_ = (lcdc_ & LCDC_WINDOW_MAP_SELECT) ? TileMap::MAP_9C00
                                                        : TileMap::MAP_9800;
    window_enabled_ = lcdc_ & LCDC_WINDOW_ENABLE;
    bg_window_addressing_mode_ = (lcdc_ & LCDC_ADDRESSING_MODE)
                                     ? AddressingMode::MODE_8000
                                     : AddressingMode::MODE_8800;
    bg_tile_map_ =
        (lcdc_ & LCDC_BG_MAP_SELECT) ? TileMap::MAP_9C00 : TileMap::MAP_9800;
    sprite_size_ = (lcdc_ & LCDC_SPRITE_SIZE) ? SpriteSize::SIZE_8X16
                                              : SpriteSize::SIZE_8X8;
    sprite_enabled_ = lcdc_ & LCDC_SPRITE_ENABLE;
    bg_window_priority_ = lcdc_ & LCDC_BG_WINDOW_PRIORITY;
}

void PPU::write_stat(memory::MemoryValue value) {
    stat_ = value & 0b11111000;
    lyc_interrupt_enabled_ = stat_ & STAT_COINCIDENCE_INTERRUPT;
    oam_scan_interrupt_enabled_ = stat_ & STAT_OAM_SCAN_INTERRUPT;
    vblank_interrupt_enabled_ = stat_ & STAT_VBLANK_INTERRUPT;
    hblank_interrupt_enabled_ = stat_ & STAT_HBLANK_INTERRUPT;
}

void PPU::write(memory::MemoryAddr addr, memory::MemoryValue value) {
    // Individual register.
    switch (addr) {
        case LCDC: write_lcdc(value); return;
        case STAT: write_stat(value); return;
        case SCY: scy_ = value; return;
        case SCX: scx_ = value; return;
        case LY: LOG(ERROR) << "Write to read-only LY, ignoring"; return;
        case LYC: lyc_ = value; return;
        case BGP: write_palette(&bgp_, value); return;
        case OBP0: write_palette(&obp0_, value); return;
        case OBP1: write_palette(&obp1_, value); return;
        case WY: wy_ = value; return;
        case WX: wx_ = value; return;
        default: break;
    }

    if (BETWEEN(memory::OAM_BEGIN, addr, memory::OAM_END)) {
        if (mode_ == Mode::HBLANK || mode_ == Mode::VBLANK) {
            oam_[addr - memory::OAM_BEGIN] = value;
        } else {
            LOG(ERROR)
                << "Writing to OAM during OAM scan or LCD draw, ignoring";
        }

        return;
    }

    if (BETWEEN(TILE_DATA_BEGIN, addr, TILE_DATA_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            tiles_[addr - TILE_DATA_BEGIN] = value;
        } else {
            LOG(ERROR) << "Writing to VRAM during LCD draw, returning dummy.";
        }

        return;
    }

    if (BETWEEN(BG_MAP_9800_BEGIN, addr, BG_MAP_9800_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            bg_map_9800_[addr - BG_MAP_9800_BEGIN] = value;
        } else {
            LOG(ERROR) << "Writing to VRAM during LCD draw, ignoring.";
        }

        return;
    }

    if (BETWEEN(BG_MAP_9C00_BEGIN, addr, BG_MAP_9C00_END)) {
        if (mode_ != Mode::LCD_DRAW) {
            bg_map_9c00_[addr - BG_MAP_9C00_BEGIN] = value;
        } else {
            LOG(ERROR) << "Writing to VRAM during LCD draw, ignoring.";
        }

        return;
    }

    LOG(ERROR) << "Invalid write to PPU, ignoring.";
}

OAMEntry PPU::get_oam_entry(uint8_t entry) {
    DCHECK(BETWEEN(0, entry, OAM_COUNT - 1));

    auto base_addr = entry * OAM_SIZE;

    OAMEntry result = {};
    result.y = oam_[base_addr];
    result.x = oam_[base_addr + 1];
    result.tile_number = oam_[base_addr + 2];

    auto flags = oam_[base_addr + 3];
    result.priority = (flags & OAM_BG_PRIORITY) ? OAMEntry::Priority::BELOW_BG
                                                : OAMEntry::Priority::ABOVE_BG;
    result.vertical_flip = (flags & OAM_Y_FLIP);
    result.horizontal_flip = (flags & OAM_X_FLIP);
    result.palette = (flags & OAM_PALETTE) ? OAMEntry::Palette::PALETTE_1
                                           : OAMEntry::Palette::PALLETE_0;

    return result;
}

std::array<std::array<uint8_t, 128>, 128> PPU::tile_data(
    AddressingMode mode) const {
    std::array<std::array<uint8_t, 128>, 128> result = {};

    for (uint16_t i = 0; i < 256; ++i) {
        Tile tile = get_tile_8x8(translate_tile_id(i, mode));

        uint8_t row = (i / 16) * 8;
        uint8_t col = (i % 16) * 8;

        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 8; ++k) {
                result[row + j][col + k] = tile.get_pixel(j, k);
            }
        }
    }

    return result;
}

Tile PPU::get_tile_8x8(size_t tile) const {
    DCHECK((0 <= tile) && (tile <= TILE_COUNT));

    std::array<uint8_t, 16> data{};
    for (int i = 0; i < 16; ++i) {
        data[i] = tiles_[tile * TILE_SIZE + i];
    }

    return Tile(data);
}

Tile PPU::get_tile_8x16(size_t tile) const {
    DCHECK((0 <= tile) && (tile <= TILE_COUNT));

    tile &= 0b11111110;

    std::array<uint8_t, 32> data{};
    for (int i = 0; i < 32; ++i) {
        data[i] = tiles_[tile * TILE_SIZE + i];
    }

    return Tile(data);
}

void PPU::update_ly() {
    ly_ = (ly_ + 1) % 154;

    if (ly_ == lyc_ && lyc_interrupt_enabled_) {
        // TODO: generate LYC interrupt
    }
}

void PPU::tick() {
    if (!lcd_enabled_) {
        return;
    }

    if (dots_remaining_ > 0) {
        dots_remaining_--;
        return;
    }

    if (dots_remaining_ == 0) {
        switch (mode_) {
            case Mode::OAM_SCAN: mode_ = Mode::LCD_DRAW;
            case Mode::LCD_DRAW: mode_ = Mode::HBLANK;
            case Mode::HBLANK:
                update_ly();
                if (ly_ == 143) {
                    mode_ = Mode::VBLANK;
                } else {
                    mode_ = Mode::OAM_SCAN;
                }
                break;
            case Mode::VBLANK:
                update_ly();
                if (ly_ == 0) {
                    mode_ = Mode::OAM_SCAN;
                }
                break;
        }
    }

    if (mode_ == Mode::OAM_SCAN) {
        oam_scan();

        // OAM scanning takes 20 cycles, minus the current dot.
        dots_remaining_ = 20 - 1;

        return;
    }

    if (mode_ == Mode::LCD_DRAW) {
        lcd_draw();

        // Takes between 168 to 291 dots, might as well take the maximum
        dots_remaining_ = 72 - 1;

        return;
    }

    if (mode_ == Mode::HBLANK) {
        dots_remaining_ = 22 - 1;

        return;
    }

    if (mode_ == Mode::VBLANK) {
        dots_remaining_ = 114 - 1;

        return;
    }
}

void PPU::oam_scan() {
    if (!sprite_enabled_) {
        // TODO: do nothing, maybe erase the OAM list?
        return;
    }

    for (uint8_t i = 0; i < OAM_COUNT; ++i) {
        OAMEntry oam = get_oam_entry(i);
    }
}

void PPU::lcd_draw() {
    auto bg = background();

    uint8_t real_y = scy_ + ly_;

}

std::array<std::array<uint8_t, 256>, 256> PPU::background() const {
    std::array<std::array<uint8_t, 256>, 256> result = {};

    // TODO: fix this mess.
    const uint8_t *tile_map = bg_map_9800_;
    if (bg_tile_map_ == TileMap::MAP_9C00) {
        tile_map = bg_map_9c00_;
    }

    for (size_t i = 0; i < BG_MAP_SIZE * BG_MAP_SIZE; ++i) {
        uint8_t tile_num =
            translate_tile_id(tile_map[i], bg_window_addressing_mode_);
        auto tile = get_tile_8x8(tile_num);

        size_t row = (i / BG_MAP_SIZE) * 8, col = (i % BG_MAP_SIZE) * 8;

        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 8; ++k) {
                result[row + j][col + k] = tile.get_pixel(j, k);
            }
        }
    }

    return result;
}

std::array<std::array<uint8_t, 256>, 256> PPU::window() const {}

}  // namespace ppu
