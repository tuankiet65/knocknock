#include "knocknock/memory/mmu.h"

#include <fmt/format.h>
#include <glog/logging.h>

namespace memory {

MMU::MMU() : regions_() {}

bool MMU::has_overlapping_regions(MemoryAddr start, MemoryAddr end) const {
    for (const MemoryRegion &region : regions_) {
        if (BETWEEN(region.start(), start, region.end()) ||
            BETWEEN(region.start(), end, region.end()))
            return true;
    }

    return false;
}

bool MMU::register_region(Memory *region, MemoryAddr start, MemoryAddr end) {
    DCHECK(start <= end) << fmt::format("end ({}) < start({})", end, start);

    if (has_overlapping_regions(start, end)) {
        LOG(ERROR) << "Region overlaps with existing regions: "
                   << "start = 0x" << std::hex << start << " "
                   << "end = 0x" << std::hex << end;
        return false;
    }

    regions_.emplace_back(region, start, end);
    return true;
}

MemoryValue MMU::read(MemoryAddr addr) const {
    for (const auto &region : regions_) {
        if (BETWEEN(region.start(), addr, region.end())) {
            return region.region()->read(addr);
        }
    }

    LOG(ERROR) << fmt::format(
        "No matching region for addr {:#04x}, returning junk value", addr);
    return 0xff;
}

void MMU::write(MemoryAddr addr, MemoryValue value) {
    for (const auto &region : regions_) {
        if (BETWEEN(region.start(), addr, region.end())) {
            region.region()->write(addr, value);
            return;
        }
    }

    LOG(ERROR) << fmt::format(
        "No matching region for addr {:#04x}, ignoring write", addr);
}

}  // namespace memory
