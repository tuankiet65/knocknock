#pragma once

#include <cstdint>
#include <stdexcept>

#define BETWEEN(l, m, r) ((l) <= (m) && (m) <= (r))

namespace memory {

using MemoryAddr = uint16_t;
using MemoryValue = uint8_t;

class MemoryAddrProxy;

class Memory {
public:
    virtual bool read(MemoryAddr addr, MemoryValue *dest) const = 0;
    virtual bool write(MemoryAddr addr, MemoryValue value) = 0;

    MemoryAddrProxy operator[](MemoryAddr addr);
};

class MemoryAddrProxy {
public:
    MemoryAddrProxy(Memory &memory, MemoryAddr addr)
        : memory_(memory), addr_(addr) {
            // DCHECK(memory)
        }

    operator MemoryValue() const {
        MemoryValue value;
        if (!memory_.read(addr_, &value))
            throw std::out_of_range("addr out of range");

        return value;
    }

    MemoryAddrProxy& operator=(MemoryValue value) {
        memory_.write(addr_, value);
        return *this;
    }

private:
    Memory &memory_;
    const MemoryAddr addr_;
};

} // namespace cpu
