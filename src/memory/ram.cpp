#include "memory/ram.h"

namespace memory {

bool RAM::read(MemoryAddr addr, MemoryValue *dest) const {
    // TODO: check addr
    *dest = ram_[addr - START_ADDR];
    return true;
}

bool RAM::write(MemoryAddr addr, MemoryValue value) {
    // TODO: check addr
    ram_[addr - START_ADDR] = value;
    return true;
}

}
