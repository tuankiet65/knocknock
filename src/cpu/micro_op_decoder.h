#pragma once

#include <queue>

#include "cpu/instruction.h"
#include "cpu/micro_op.h"

namespace cpu {

class MicroOpDecoder {
public:
    MicroOpDecoder() = delete;

    MicroOpDecoder(const MicroOpDecoder &) = delete;
    MicroOpDecoder &operator=(const MicroOpDecoder &) = delete;

    static void decode(Instruction inst, std::queue<MicroOp> *uop_queue);
};

}  // namespace cpu
