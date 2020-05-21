#pragma once

#include "knocknock/memory/memory.h"

namespace memory {

/**
 * A memory that allows read and write access everywhere in its entire 16-bit
 * memory space (0x0000 - 0xffff). Shall only be used in unittests so one
 * wouldn't need to care about the valid address space.
 */
class TestMemory : public ROMLoadableMemory {
public:
    TestMemory();

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;
    bool load_rom(const std::vector<MemoryValue> &rom) override;

private:
    MemoryValue memory_[0x10000];
};

}  // namespace memory
