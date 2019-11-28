#pragma once

#include "cpu/operands.h"
#include "memory/memory.h"

namespace cpu {

class CPU {
public:
    CPU(memory::Memory *mem);

private:
    Register8 a_, b_, c_, d_, e_, h_, l_;
    FlagRegister f_;
    Register16Mirror af_, bc_, de_, hl_;
    Register16 sp_;

    memory::Memory *mem_;
    memory::MemoryAddr pc_;
};

}  // namespace cpu
