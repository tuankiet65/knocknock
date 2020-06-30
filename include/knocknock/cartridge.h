#pragma once

#include <optional>
#include <string>
#include <vector>

#include "knocknock/base/filesystem.h"
#include "knocknock/memory/memory.h"

using CartridgeContent = std::vector<memory::MemoryValue>;

class Cartridge {
public:
    enum class GameBoyType { GameBoy, ColorGameBoy };

    enum class CartridgeType {
        ROM = 0x00,

        ROM_MBC1 = 0x01,
        ROM_MBC1_RAM = 0x02,
        ROM_MBC1_RAM_BATT = 0x03,

        ROM_MBC2 = 0x05,
        ROM_MBC2_BATT = 0x06,

        ROM_RAM = 0x08,
        ROM_RAM_BATT = 0x09,

        ROM_MMM01 = 0x0b,
        ROM_MMM01_SRAM = 0x0c,
        ROM_MMM01_SRAM_BATT = 0x0d,

        ROM_MBC3_RTC_BATT = 0x0f,
        ROM_MBC3_RTC_RAM_BATT = 0x10,
        ROM_MBC3 = 0x11,
        ROM_MBC3_RAM = 0x12,
        ROM_MBC3_RAM_BATT = 0x13,

        ROM_MBC5 = 0x19,
        ROM_MBC5_RAM = 0x1a,
        ROM_MBC5_RAM_BATT = 0x1b,
        ROM_MBC5_RUMBLE = 0x1c,
        ROM_MBC5_RUMBLE_SRAM = 0x1d,
        ROM_MBC5_RUMBLE_SRAM_BATT = 0x1e,

        POCKET_CAMERA = 0x1f,
        BANDAI_TAMA5 = 0xfd,
        HUDSON_HUC3 = 0xfe,
        HUDSON_HUC1 = 0xff
    };

    Cartridge(std::string title,
              GameBoyType game_boy_type,
              CartridgeType type,
              size_t rom_size,
              size_t ram_size,
              CartridgeContent content_);

    static std::optional<Cartridge> from_file(const fs::path &path);

    CartridgeContent content() const { return content_; }
    std::string title() const { return title_; }
    GameBoyType game_boy_type() const { return game_boy_type_; }
    CartridgeType type() const { return type_; }
    size_t rom_size() const { return rom_size_; }
    size_t ram_size() const { return ram_size_; }

private:
    std::string title_;
    GameBoyType game_boy_type_;
    CartridgeType type_;
    size_t rom_size_;
    size_t ram_size_;
    CartridgeContent content_;
};
