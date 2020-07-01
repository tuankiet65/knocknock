#include "knocknock/memory/test_memory.h"

namespace memory {

TestMemory::TestMemory() : memory_() {}

MemoryValue TestMemory::read(MemoryAddr addr) const {
    return memory_[addr];
}

void TestMemory::write(MemoryAddr addr, MemoryValue value) {
    memory_[addr] = value;
}

}  // namespace memory
