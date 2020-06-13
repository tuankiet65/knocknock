#pragma once

namespace ppu {

struct OAMEntry {
    /**
     * The y coordinate.
     */
    int y;

    /**
     * The x coordinate.
     */
    int x;

    /**
     * The tile that this entry refers to.
     */
    int tile_number;

    /**
     * True if the tile should be flipped vertically.
     */
    bool vertical_flip;

    /**
     * True if the tile should be flipped horizontally.
     */
    bool horizontal_flip;

    enum class Palette { PALLETE_0, PALETTE_1 };

    /**
     * Whether the color is interpreted as palette 0 or 1.
     */
    Palette palette;

    enum class Priority { ABOVE_BG, BELOW_BG };

    Priority priority;
};

}  // namespace ppu
