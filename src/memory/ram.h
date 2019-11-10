#pragma once

#include "memory/memory.h"

namespace memory {

class RAM : public Memory {
public:
    static const uint16_t START_ADDR = 0xc000;
    static const uint16_t END_ADDR = 0xdfff;

    // Memory overrides
    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

private:
    uint8_t ram_[END_ADDR - START_ADDR + 1];
};

}  // namespace memory
