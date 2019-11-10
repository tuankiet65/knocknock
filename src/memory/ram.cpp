#include "memory/ram.h"

#include <glog/logging.h>

namespace memory {

MemoryValue RAM::read(MemoryAddr addr) const {
    DCHECK(BETWEEN(START_ADDR, addr, END_ADDR))
        << "addr out of range for RAM: 0x" << std::hex << addr;

    return ram_[addr - START_ADDR];
}

void RAM::write(MemoryAddr addr, MemoryValue value) {
    DCHECK(BETWEEN(START_ADDR, addr, END_ADDR))
        << "addr out of range for RAM: 0x" << std::hex << addr;

    ram_[addr - START_ADDR] = value;
}

}  // namespace memory
