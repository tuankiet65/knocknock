#pragma once

#include <memory>
#include <string>
#include <vector>

#include "filesystem.h"
#include "memory/memory.h"

using CartridgeContent = std::vector<memory::MemoryValue>;

class Cartridge {
public:
    enum GameBoyType { GameBoy, ColorGameBoy };

    // enum SuperGameBoyIndicator{
    //     SuperGameBoyEnabled,
    //     SuperGameBoyDisabled
    // };

    enum CartridgeType {
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

        ROM_MBC3_TIMER_BATT = 0x0f,
        ROM_MBC3_TIMER_RAM_BATT = 0x10,
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

    // enum ROMSize {
    //     ROM_256K = 0x00,
    //     ROM_512K = 0x01,
    //     ROM_1M = 0x02,
    //     ROM_2M = 0x03,
    //     ROM_4M = 0x04,
    //     ROM_8M = 0x05,
    //     ROM_16M = 0x06,
    //     ROM_9M = 0x52,
    //     ROM_10M = 0x53,
    //     ROM_12M = 0x54
    // };

    // enum RAMSize {
    //     RAM_NONE = 0x00,
    //     RAM_16K = 0x01,
    //     RAM_64K = 0x02,
    //     RAM_256K = 0x03,
    //     RAM_1M = 0x04
    // };

    // enum Region {
    //     Japanese = 0x00,
    //     NonJapanese = 0x01
    // };

    Cartridge(const std::string &title,
              GameBoyType game_boy_type,
              /* uint8_t licensee_code, */
              /* SuperGameBoyIndicator sbg_indicator, */
              CartridgeType type,
              size_t rom_size,
              size_t ram_size,
              /* Region region, */
              /* uint8_t mask_rom_version, */
              const CartridgeContent &content_);

    static bool from_file(fs::path path, std::unique_ptr<Cartridge> *cartridge);

    CartridgeContent content() const { return content_; }
    std::string title() const { return title_; }
    GameBoyType game_boy_type() const { return game_boy_type_; }
    // uint8_t licensee_code() const { return licensee_code_; }
    // SuperGameBoyIndicator sbg_indicator() const { return sbg_indicator_; }
    CartridgeType type() const { return type_; }
    size_t rom_size() const { return rom_size_; }
    size_t ram_size() const { return ram_size_; }
    // Region region() const { return region_; }
    // uint8_t mask_rom_version() const { return mask_rom_version_; }

private:
    CartridgeContent content_;
    std::string title_;
    GameBoyType game_boy_type_;
    // uint8_t licensee_code_;
    // SuperGameBoyIndicator sbg_indicator_;
    CartridgeType type_;
    size_t rom_size_;
    size_t ram_size_;
    // Region region_;
    // uint8_t mask_rom_version_;
};
