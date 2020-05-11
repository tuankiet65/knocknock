#include "memory/memory.h"

#include <glog/logging.h>

namespace memory {

MemoryAddrProxy Memory::operator[](MemoryAddr addr) {
    return MemoryAddrProxy(this, addr);
}

uint16_t Memory::read16(MemoryAddr addr) const {
    // Gameboy is little-endian => first byte is LSB, second byte is MSB
    uint16_t result = read(addr);
    result = (read(addr + 1) << 8) | result;

    return result;
}

void Memory::write16(MemoryAddr addr, uint16_t value) {
    // Gameboy is little-endian => first byte is LSB, second byte is MSB
    write(addr, value & 0b0000000011111111);  // LSB by taking the last 8 bits
    write(addr + 1, value >> 8);  // MSB by discarding the last 8 bits
}

MemoryAddrProxy::MemoryAddrProxy(Memory *memory, MemoryAddr addr)
    : memory_(memory), addr_(addr) {
    DCHECK(memory);
}

MemoryAddrProxy::operator MemoryValue() const {
    return memory_->read(addr_);
}

MemoryValue MemoryAddrProxy::operator*() const {
    return memory_->read(addr_);
}

MemoryAddrProxy &MemoryAddrProxy::operator=(MemoryValue value) {
    memory_->write(addr_, value);
    return *this;
}

}  // namespace memory
