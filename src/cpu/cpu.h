#pragma once

#include "register.h"
#include "memory.h"

namespace cpu {

class CPU {
private:
    Register r;
    Memory mem;
};

} // namespace cpu
