#pragma once

#include <cstdint>
#include <stdexcept>

#define BETWEEN(l, m, r) ((l) <= (m) && (m) <= (r))

namespace memory {

using MemoryAddr = uint16_t;
using MemorySize = uint16_t;
using MemoryValue = uint8_t;

class MemoryAddrProxy;

class Memory {
public:
    virtual MemoryValue read(MemoryAddr addr) const = 0;
    virtual void write(MemoryAddr addr, MemoryValue value) = 0;

    // Helper function to read and write 16-bit values using read() and write()
    uint16_t read16(MemoryAddr addr) const;
    void write16(MemoryAddr addr, uint16_t value);

    MemoryAddrProxy operator[](MemoryAddr addr);
};

class BulkLoadableMemory : public Memory {
public:
    virtual bool bulk_load(MemoryAddr addr, MemoryValue data[], size_t len) = 0;
};

class MemoryAddrProxy {
public:
    MemoryAddrProxy(Memory &memory, MemoryAddr addr)
        : memory_(memory), addr_(addr) {
            // DCHECK(memory)
        }

    operator MemoryValue() const {
        return memory_.read(addr_);
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
