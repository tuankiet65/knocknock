#pragma once
#include <glog/logging.h>

#include <functional>
#include <string>

#include "cpu/operands.h"
#include "memory/memory.h"

namespace cpu {

class CPU {
public:
    CPU(memory::Memory *mem);

    // To be defined outside
    class Instruction;

    void step();

    Instruction decode();

private:
    Register8 a_, b_, c_, d_, e_, h_, l_;
    FlagRegister f_;
    Register16Mirror af_, bc_, de_, hl_;
    Register16 sp_, pc_;

    memory::Memory *mem_;

    int cycles_remaining_;
};

class CPU::Instruction {
public:
    Instruction(std::string disassembly, int cycles, std::function<void()> func)
        : disassembly_(disassembly),
          cycles_(cycles),
          func_(func),
          executed_(false) {}

    void execute() {
        DCHECK(executed_) << "Attempt to execute instruction more than once";

        func_();
        executed_ = true;
    }

    std::string disassembly() const { return disassembly_; }
    int cycles() const { return cycles_; }
    bool executed() const { return executed_; }

private:
    // The disassembly of the instruction
    std::string disassembly_;
    // Number of cycles for the instruction to finish executing
    int cycles_;
    // Function to be invoked
    std::function<void()> func_;
    // Whether the instruction has already been executed
    bool executed_;
};

}  // namespace cpu
