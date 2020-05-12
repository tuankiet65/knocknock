#pragma once

#include <vector>

#include "memory/memory.h"
#include "memory/regions.h"

namespace memory::testing {

using BankValuePair = std::pair<uint8_t, uint8_t>;

std::vector<MemoryValue> generate_test_rom(
    MemorySize banks,
    std::initializer_list<BankValuePair> bank_value_pairs);

bool verify_rom_0_value(const Memory& memory, MemoryValue value);

bool verify_rom_switchable_value(const Memory& memory, MemoryValue value);

}  // namespace memory::testing.