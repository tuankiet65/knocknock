#pragma once

#include <vector>

#include "memory/memory.h"

namespace memory::testing {

using BankValuePair = std::pair<uint8_t, uint8_t>;

std::vector<MemoryValue> generate_test_rom(
    MemorySize banks,
    std::initializer_list<BankValuePair> bank_value_pairs);

}  // namespace memory::testing.