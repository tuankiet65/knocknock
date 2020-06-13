#pragma once

#include "knocknock/memory/memory.h"
#include "knocknock/memory/mmu.h"
#include "knocknock/memory/regions.h"
#include "knocknock/peripherals/tickable.h"

#include "oam_entry.h"
#include "tile.h"

namespace ppu {

using Palette = std::array<uint8_t, 4>;

class PPU : public memory::Memory, public peripherals::Tickable {
public:
    PPU();

    void register_to_mmu(memory::MMU *mmu);

    // memory::Memory::
    memory::MemoryValue read(memory::MemoryAddr addr) const override;
    void write(memory::MemoryAddr addr, memory::MemoryValue value) override;

    // peripherals::Tickable::
    void tick() override;

    enum class AddressingMode { MODE_8000, MODE_8800 };

    enum class TileMap {
        MAP_9800,  // 0x9800 - 0x9bff
        MAP_9C00   // 0x9c00 - 0x9fff
    };

    enum class SpriteSize { SIZE_8X8, SIZE_8X16 };

    std::array<std::array<uint8_t, 128>, 128> tile_data(
        AddressingMode mode) const;

    std::array<std::array<uint8_t, 256>, 256> background() const;
    std::array<std::array<uint8_t, 256>, 256> window() const;

    PPU(const PPU&) = delete;
    PPU &operator=(const PPU&) = delete;

private:
    void update_ly();

    // LCDC register
    uint8_t lcdc_;
    bool lcd_enabled_;
    TileMap window_tile_map_;
    bool window_enabled_;
    AddressingMode bg_window_addressing_mode_;
    TileMap bg_tile_map_;
    SpriteSize sprite_size_;
    bool sprite_enabled_;
    bool bg_window_priority_;
    memory::MemoryValue read_lcdc() const;
    void write_lcdc(memory::MemoryValue value);

    // STAT register.
    uint8_t stat_;
    bool lyc_interrupt_enabled_;
    bool oam_scan_interrupt_enabled_;
    bool vblank_interrupt_enabled_;
    bool hblank_interrupt_enabled_;
    enum class Mode {
        HBLANK = 0,
        VBLANK = 1,
        OAM_SCAN = 2,
        LCD_DRAW = 3
    } mode_;
    memory::MemoryValue read_stat() const;
    void write_stat(memory::MemoryValue value);

    uint8_t scy_, scx_;
    uint8_t ly_, lyc_;
    uint8_t wy_, wx_;

    Palette bgp_, obp1_, obp0_;

    // Object Attribute Map or OAM.
    static constexpr size_t OAM_COUNT = 40;
    static constexpr size_t OAM_SIZE = 4;
    uint8_t oam_[OAM_COUNT * OAM_SIZE];
    OAMEntry get_oam_entry(uint8_t entry);

    // Tile data.
    static constexpr size_t TILE_SIZE = 16;
    static constexpr size_t TILE_COUNT = 384;
    uint8_t tiles_[TILE_SIZE * TILE_COUNT];
    [[nodiscard]] Tile get_tile_8x8(size_t tile) const;
    [[nodiscard]] Tile get_tile_8x16(size_t tile) const;

    // Background map.
    static constexpr size_t BG_MAP_SIZE = 32;
    uint8_t bg_map_9800_[BG_MAP_SIZE * BG_MAP_SIZE],
        bg_map_9c00_[BG_MAP_SIZE * BG_MAP_SIZE];

    int dots_remaining_;

    void oam_scan();

    static constexpr uint8_t FRAME_WIDTH = 160;
    static constexpr uint8_t FRAME_HEIGHT = 144;
    std::array<std::array<uint8_t, FRAME_WIDTH>, FRAME_HEIGHT> frame_;
    void lcd_draw();
};

}  // namespace ppu
