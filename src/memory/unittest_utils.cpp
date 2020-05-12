#include "memory/unittest_utils.h"

namespace memory::testing {

namespace {

constexpr memory::MemorySize ROM_BANK_SIZE = 0x4000;

}  // namespace

std::vector<MemoryValue> generate_test_rom(
    MemorySize banks,
    std::initializer_list<BankValuePair> bank_value_pairs) {
    std::vector<MemoryValue> rom(banks * ROM_BANK_SIZE);

    for (const BankValuePair &bank_value_pair : bank_value_pairs) {
        uint8_t bank = bank_value_pair.first;
        uint8_t value = bank_value_pair.second;

        std::fill(rom.begin() + (bank * ROM_BANK_SIZE),
                  rom.begin() + (bank * ROM_BANK_SIZE) + ROM_BANK_SIZE, value);
    }

    return rom;
}

bool verify_rom_0_value(const Memory& memory, MemoryValue value) {
    for (auto i = ROM_0_BEGIN; i <= ROM_0_END; ++i) {
        if (memory.read(i) != value) {
            return false;
        }
    }

    return true;
}

bool verify_rom_switchable_value(const Memory& memory, MemoryValue value) {
    for (auto i = ROM_SWITCHABLE_BEGIN; i <= ROM_SWITCHABLE_END; ++i) {
        if (memory.read(i) != value) {
            return false;
        }
    }

    return true;
}

}  // namespace memory::testing
