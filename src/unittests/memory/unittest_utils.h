#pragma once

#include <vector>

#include "memory/memory.h"
#include "memory/regions.h"

namespace memory::testing {

using BankValuePair = std::pair<uint8_t, uint8_t>;

std::vector<MemoryValue> generate_test_rom(
    MemorySize banks,
    std::initializer_list<BankValuePair> bank_value_pairs);

[[nodiscard]]
bool verify_rom_0_value(const Memory& memory, MemoryValue value);

[[nodiscard]]
bool verify_rom_switchable_value(const Memory& memory, MemoryValue value);

void fill_external_ram(Memory *mem, uint8_t value);

[[nodiscard]]
bool verify_external_ram_value(const Memory &memory, uint8_t value);

}  // namespace memory::testing.