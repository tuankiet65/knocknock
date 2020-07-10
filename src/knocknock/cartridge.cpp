#include "knocknock/cartridge.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <fstream>

namespace {

constexpr size_t TITLE_BEGIN_ADDR = 0x0134;
constexpr size_t TITLE_END_ADDR = 0x0142;
constexpr size_t GAMEBOY_TYPE_ADDR = 0x0143;
constexpr size_t CARTRIDGE_TYPE_ADDR = 0x0147;
constexpr size_t ROM_SIZE_ADDR = 0x0148;
constexpr size_t RAM_SIZE_ADDR = 0x0149;
constexpr size_t HEADER_BEGIN_ADDR = 0x0134;
constexpr size_t HEADER_END_ADDR = 0x014c;
constexpr size_t HEADER_CHECKSUM_ADDR = 0x014d;

constexpr size_t KBYTE = 1024;

std::string get_title(const CartridgeContent &content) {
    std::string result;

    for (size_t pos = TITLE_BEGIN_ADDR; pos <= TITLE_END_ADDR; ++pos) {
        uint8_t byte = content.at(pos);
        if (byte == 0)
            break;
        result.push_back(byte);
    }

    return result;
}

Cartridge::GameBoyType get_gameboy_type(const CartridgeContent &content) {
    uint8_t raw_type = content.at(GAMEBOY_TYPE_ADDR);

    if (raw_type == 0x80) {
        return Cartridge::GameBoyType::ColorGameBoy;
    }

    return Cartridge::GameBoyType::GameBoy;
}

bool get_rom_size(const CartridgeContent &content, size_t *result) {
    uint8_t raw_type = content.at(ROM_SIZE_ADDR);

    switch (raw_type) {
        case 0x00: *result = 32 * KBYTE; break;
        case 0x01: *result = 64 * KBYTE; break;
        case 0x02: *result = 128 * KBYTE; break;
        case 0x03: *result = 256 * KBYTE; break;
        case 0x04: *result = 512 * KBYTE; break;
        case 0x05: *result = 1024 * KBYTE; break;
        case 0x06: *result = 2048 * KBYTE; break;
        case 0x52: *result = 1152 * KBYTE; break;  // 1.1MByte
        case 0x53: *result = 1280 * KBYTE; break;  // 1.2MByte
        case 0x54: *result = 1536 * KBYTE; break;  // 1.5MByte

        default:
            LOG(ERROR) << fmt::format("Unknown ROM size {0:#x}", raw_type);
            return false;
    }

    return true;
}

bool get_ram_size(const CartridgeContent &content, size_t *result) {
    uint8_t raw_type = content.at(RAM_SIZE_ADDR);

    switch (raw_type) {
        case 0x00: *result = 0; break;
        case 0x01: *result = 2 * KBYTE; break;
        case 0x02: *result = 8 * KBYTE; break;
        case 0x03: *result = 32 * KBYTE; break;
        case 0x04: *result = 128 * KBYTE; break;

        default:
            LOG(ERROR) << fmt::format("Unknown RAM size {0:#x}", raw_type);
            return false;
    }

    return true;
}

Cartridge::CartridgeType get_type(const CartridgeContent &content) {
    uint8_t raw_type = content.at(CARTRIDGE_TYPE_ADDR);

    return static_cast<Cartridge::CartridgeType>(raw_type);
}

bool verify_header_checksum(const CartridgeContent &content) {
    uint8_t checksum = content.at(HEADER_CHECKSUM_ADDR);

    uint8_t calculated_checksum = 0;
    for (size_t i = HEADER_BEGIN_ADDR; i <= HEADER_END_ADDR; ++i) {
        calculated_checksum += content.at(i);
    }
    calculated_checksum += 0x19 + checksum;

    return (calculated_checksum == 0);
}

}  // namespace

Cartridge::Cartridge(std::string title,
                     GameBoyType game_boy_type,
                     CartridgeType type,
                     size_t rom_size,
                     size_t ram_size,
                     CartridgeContent content)
    : title_(std::move(title)),
      game_boy_type_(game_boy_type),
      type_(type),
      rom_size_(rom_size),
      ram_size_(ram_size),
      content_(std::move(content)) {}

// static
std::optional<Cartridge> Cartridge::from_file(const std::string &path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in) {
        LOG(ERROR) << fmt::format(FMT_STRING("Unable to open file {}"), path);
        return {};
    }

    CartridgeContent content;
    while (true) {
        const size_t BUFFER_SIZE = 2048;
        char buffer[BUFFER_SIZE];

        in.read(buffer, BUFFER_SIZE);
        const size_t read_count = in.gcount();

        if (in.eof()) {
            LOG(INFO) << "Reached the end of file";
            break;
        }

        if (in.fail()) {
            LOG(ERROR) << fmt::format(FMT_STRING("Error while reading file {}"),
                                      path);
            return {};
        }

        // Append the entire buffer to the end of content
        content.insert(content.end(), buffer, buffer + read_count);
    }

    // The information area is located between 0x0100 and 0x014f
    if (content.size() < 0x0150) {
        LOG(ERROR) << fmt::format(
            "Cartridge size too small to contain the information area, "
            "read size = {0:#x} bytes",
            content.size());
        return {};
    }

    std::string title = get_title(content);
    GameBoyType gameboy_type = get_gameboy_type(content);
    CartridgeType type = get_type(content);

    size_t rom_size;
    if (!get_rom_size(content, &rom_size)) {
        LOG(ERROR) << "Unable to get ROM size";
        return {};
    }

    size_t ram_size;
    if (!get_ram_size(content, &ram_size)) {
        LOG(ERROR) << "Unable to get RAM size";
        return {};
    }

    if (!verify_header_checksum(content)) {
        LOG(WARNING) << "Wrong header checksum";
    }

    return Cartridge(title, gameboy_type, type, rom_size, ram_size, content);
}
