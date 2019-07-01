#pragma once

#include <vector>

#include "memory/memory.h"

namespace memory {

class MemoryRegion {
public:
    MemoryRegion(int type, Memory *region, MemoryAddr start, MemoryAddr end)
        : type_(type), region_(region), start_(start), end_(end) {}

    int type()         const { return type_; }
    Memory* region()   const { return region_; }
    MemoryAddr start() const { return start_; }
    MemoryAddr end()   const { return end_; }
    MemoryAddr size()  const { return (end_ - start_ + 1); }

private:
    int type_;
    Memory *region_;
    MemoryAddr start_;
    MemoryAddr end_;
};

class MMU : public Memory {
public:
    // Memory overrides
    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

    bool add_region(int type, Memory *region, MemoryAddr start, MemoryAddr end);

    const std::vector<MemoryRegion>& regions() const { return regions_; }

private:
    bool region_does_overlap(MemoryAddr start, MemoryAddr end) const;

    std::vector<MemoryRegion> regions_;
};

}  // namespace memory
