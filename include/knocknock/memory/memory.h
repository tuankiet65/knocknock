#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#define BETWEEN(l, m, r) ((l) <= (m) && (m) <= (r))

namespace memory {

using MemoryAddr = uint16_t;
using MemorySize = uint32_t;
using MemoryValue = uint8_t;

class Memory {
private:
    class Proxy {
    public:
        Proxy(Memory *memory, MemoryAddr addr);

        operator MemoryValue() const;
        MemoryValue operator*() const;

        Proxy &operator=(MemoryValue value);

    private:
        Memory *memory_;
        const MemoryAddr addr_;

        Proxy(const Proxy &) = delete;
        Proxy &operator=(Proxy other) = delete;
    };

public:
    virtual MemoryValue read(MemoryAddr addr) const = 0;
    virtual void write(MemoryAddr addr, MemoryValue value) = 0;

    virtual ~Memory() {}

    // Helper function to read and write 16-bit values using read() and write()
    uint16_t read16(MemoryAddr addr) const;
    void write16(MemoryAddr addr, uint16_t value);

    Proxy operator[](MemoryAddr addr);
};

class ROMLoadableMemory : public Memory {
public:
    virtual bool load_rom(const std::vector<MemoryValue> &rom) = 0;

    virtual ~ROMLoadableMemory() {}
};

}  // namespace memory
