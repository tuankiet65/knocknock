#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace ppu {

using Palette = std::array<uint8_t, 4>;

class Tile {
public:
    // 8x8 tile constructor.
    explicit Tile(std::array<uint8_t, 8 * 2> data);
    // 8x16 tile constructor.
    explicit Tile(std::array<uint8_t, 16 * 2> data);

    [[nodiscard]] uint8_t get_pixel(uint8_t row, uint8_t col) const;
    void flip_x();
    void flip_y();
    void apply_palette(Palette palette);

private:
    static constexpr uint8_t WIDTH = 8;
    const uint8_t height_;

    std::vector<std::array<uint8_t, WIDTH>> pixels_;
};

}  // namespace ppu
