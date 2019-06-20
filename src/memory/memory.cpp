#include "memory/memory.h"

namespace memory {

MemoryAddrProxy Memory::operator[](MemoryAddr addr) {
    return MemoryAddrProxy(*this, addr);
}

} // namespace memory
