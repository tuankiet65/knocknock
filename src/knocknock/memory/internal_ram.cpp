#include "knocknock/memory/internal_ram.h"

#include "glog/logging.h"

namespace memory {

InternalRAM::InternalRAM() : ram_(), hram_() {}

MemoryValue InternalRAM::read(MemoryAddr addr) const {
    if (BETWEEN(RAM_INTERNAL_BEGIN, addr, RAM_INTERNAL_END)) {
        return ram_[addr - RAM_INTERNAL_BEGIN];
    }

    if (BETWEEN(RAM_ECHO_BEGIN, addr, RAM_ECHO_END)) {
        return ram_[addr - RAM_ECHO_BEGIN];
    }

    if (BETWEEN(HRAM_BEGIN, addr, HRAM_END)) {
        return hram_[addr - HRAM_BEGIN];
    }

    DCHECK(false) << "Invalid write to InternalRAM";
    return 0xff;
}

void InternalRAM::write(MemoryAddr addr, MemoryValue value) {
    if (BETWEEN(RAM_INTERNAL_BEGIN, addr, RAM_INTERNAL_END)) {
        ram_[addr - RAM_INTERNAL_BEGIN] = value;
        return;
    }

    if (BETWEEN(RAM_ECHO_BEGIN, addr, RAM_ECHO_END)) {
        ram_[addr - RAM_ECHO_BEGIN] = value;
        return;
    }

    if (BETWEEN(HRAM_BEGIN, addr, HRAM_END)) {
        hram_[addr - HRAM_BEGIN] = value;
        return;
    }

    DCHECK(false) << "Invalid write to InternalRAM";
}

}
