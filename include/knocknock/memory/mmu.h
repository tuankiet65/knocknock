#pragma once

#include <vector>

#include "knocknock/memory/memory.h"

namespace memory {

class MMU : public Memory {
public:
    MMU();

    bool register_region(Memory *region, MemoryAddr start, MemoryAddr end);

    // Memory::
    MemoryValue read(MemoryAddr addr) const override;
    void write(MemoryAddr addr, MemoryValue value) override;

private:
    bool has_overlapping_regions(MemoryAddr start, MemoryAddr end) const;

    class MemoryRegion {
    public:
        MemoryRegion(Memory *region, MemoryAddr start, MemoryAddr end)
            : region_(region), start_(start), end_(end) {}

        Memory *region() const { return region_; }
        MemoryAddr start() const { return start_; }
        MemoryAddr end() const { return end_; }
        MemoryAddr size() const { return (end_ - start_ + 1); }

    private:
        Memory *region_;
        MemoryAddr start_;
        MemoryAddr end_;
    };

    std::vector<MemoryRegion> regions_;
};

}  // namespace memory
