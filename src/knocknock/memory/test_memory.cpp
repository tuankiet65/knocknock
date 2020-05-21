#include "knocknock/memory/test_memory.h"

namespace memory {

TestMemory::TestMemory() : memory_() {}

MemoryValue TestMemory::read(MemoryAddr addr) const {
    return memory_[addr];
}

void TestMemory::write(MemoryAddr addr, MemoryValue value) {
    memory_[addr] = value;
}

bool TestMemory::load_rom(const std::vector<MemoryValue> &rom) {
    if (rom.size() > sizeof(memory_)) {
        return false;
    }

    std::copy(rom.begin(), rom.end(), memory_);
    return true;
}

}  // namespace memory
