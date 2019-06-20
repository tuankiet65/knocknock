#include "memory/ram.h"

#include <glog/logging.h>

namespace memory {

bool RAM::read(MemoryAddr addr, MemoryValue *dest) const {
    DCHECK(BETWEEN(START_ADDR, addr, END_ADDR)) 
        << "addr out of range for RAM: 0x" << std::hex << addr;

    *dest = ram_[addr - START_ADDR];
    return true;
}

bool RAM::write(MemoryAddr addr, MemoryValue value) {
    DCHECK(BETWEEN(START_ADDR, addr, END_ADDR)) 
        << "addr out of range for RAM: 0x" << std::hex << addr;

    ram_[addr - START_ADDR] = value;
    return true;
}

}
