#include "memory/mmu.h"

#include <algorithm>
#include <functional>

#include <glog/logging.h>

namespace memory {

namespace {

using AddrInRegionFunction = std::function<bool(const MemoryRegion&)>;

AddrInRegionFunction addr_in_region(MemoryAddr addr) {
    return [addr](const MemoryRegion& region) -> bool {
        return BETWEEN(region.start(), addr, region.end());
    };
}

} // namespace

bool MMU::region_does_overlap(MemoryAddr start, MemoryAddr end) const {
    for (const MemoryRegion& region: regions_) {
        if (BETWEEN(region.start(), start, region.end()) ||
            BETWEEN(region.start(), end, region.end()))
            return true;
    }

    return false;
}

bool MMU::add_region(int type, Memory *region,
                     MemoryAddr start, MemoryAddr end) {
    if (region_does_overlap(start, end)) {
        LOG(ERROR) << "Region overlaps with existing regions: "
                   << "type = " << type << " "
                   << "start = 0x" << std::hex << start << " "
                   << "end = 0x" << std::hex << end;
       return true;
    }

    regions_.push_back(MemoryRegion(type, region, start, end));
    return true;
}

bool MMU::read(MemoryAddr addr, MemoryValue *dest) const {
    auto region_iter =
        std::find_if(regions_.begin(), regions_.end(), addr_in_region(addr));

    if (region_iter == regions_.end()) {
        LOG(ERROR) << "No matching region for addr: 0x" << std::hex << addr;
        return false;
    }

    return region_iter->region()->read(addr, dest);
}

bool MMU::write(MemoryAddr addr, MemoryValue value) {
    auto region_iter =
        std::find_if(regions_.begin(), regions_.end(), addr_in_region(addr));

    if (region_iter == regions_.end()) {
        LOG(ERROR) << "No matching region for addr: 0x" << std::hex << addr;
        return false;
    }

    return region_iter->region()->write(addr, value);
}

} // namespace memory
