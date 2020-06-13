#include "knocknock/ppu/tile.h"

#include <algorithm>

#include <glog/logging.h>

namespace ppu {

namespace {

inline bool BETWEEN(int x, int y, int z) {
    return (x <= y) && (y <= z);
}

}  // namespace

Tile::Tile(std::array<uint8_t, 8 * 2> data)
    : height_(8), pixels_(8) /* Allocate 8 std::arrays for each row */ {
    for (uint8_t row = 0; row < WIDTH; ++row) {
        // A 8xN tile is encoded as a list of N*2 uint8_ts, each two uint8_ts
        // describes one row of pixel. The first uint8_t contains the high
        // bit of the pixel, the second uint8_t contains the low bit of the
        // pixel. For example these uint8_ts describes row 0.
        // hi  = 01010100
        // low = 01010001
        // Then pixel (0, 0) = 0b00 = 0, pixel (0, 1) = 0b11 = 3 and so on.

        uint8_t low = data[2 * row];
        uint8_t hi = data[2 * row + 1];

        for (uint8_t col = 0; col < height_; ++col) {
            uint8_t color = ((hi & 0x1) << 1) | (low & 0x1);
            low >>= 1;
            hi >>= 1;
            pixels_[row][WIDTH - col - 1] = color;
        }
    }
}

Tile::Tile(std::array<uint8_t, 16 * 2> data) : height_(16), pixels_(16) {
    for (uint8_t row = 0; row < WIDTH; ++row) {
        uint8_t low = data[2 * row];
        uint8_t hi = data[2 * row + 1];

        for (uint8_t col = 0; col < height_; ++col) {
            uint8_t color = ((hi & 0x1) << 1) | (low & 0x1);
            low >>= 1;
            hi >>= 1;
            pixels_[row][WIDTH - col - 1] = color;
        }
    }
}

uint8_t Tile::get_pixel(uint8_t row, uint8_t col) const {
    DCHECK(BETWEEN(0, row, height_ - 1));
    DCHECK(BETWEEN(0, col, WIDTH - 1));
    return pixels_[row][col];
}

void Tile::flip_x() {
    for (uint8_t row = 0; row < height_ / 2; ++row) {
        std::swap(pixels_[row], pixels_[height_ - row - 1]);
    }
}

void Tile::flip_y() {
    for (uint8_t row = 0; row < height_; ++row) {
        std::reverse(pixels_[row].begin(), pixels_[row].end());
    }
}

void Tile::apply_palette(Palette palette) {
    for (uint8_t row = 0; row < height_; ++row) {
        for (uint8_t col = 0; col < WIDTH; ++col) {
            DCHECK(BETWEEN(0, pixels_[row][col], 3));
            pixels_[row][col] = palette[pixels_[row][col]];
        }
    }
}

}  // namespace ppu
