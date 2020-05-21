#pragma once

#include "knocknock/memory/memory.h"
#include "knocknock/memory/regions.h"

namespace memory {

class InternalRAM : public memory::Memory {
public:
    InternalRAM();

    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

private:
    MemoryValue ram_[RAM_INTERNAL_SIZE];
    MemoryValue hram_[HRAM_SIZE];
};

}
