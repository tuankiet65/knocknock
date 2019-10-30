#pragma once

#include <string>

#include "memory/memory.h"

namespace cpu {

template <class T>
class Operand {
public:
    virtual T read() const = 0;
    virtual void write(T value) = 0;
};

using Operand8 = Operand<uint8_t>;
using Operand16 = Operand<uint16_t>;

class Register8 : public Operand8 {
public:
    Register8();

    uint8_t read() const override;
    void write(uint8_t value) override;

private:
    uint8_t value_;
};

class Register16 : public Operand16 {
public:
    Register16();

    uint16_t read() const override;
    void write(uint16_t value) override;

private:
    uint16_t value_;
};

class Register16Mirror : public Register16 {
public:
    Register16Mirror(Register8 *msb, Register8 *lsb);

    uint16_t read() const override;
    void write(uint16_t value) override;

private:
    Register8 *msb_, *lsb_;
};

class Immediate8 : public Operand8 {
public:
    Immediate8(uint8_t value);

    uint8_t read() const override;
    void write(uint8_t value) override;

private:
    const uint8_t value_;
};

class Immediate16 : public Operand16 {
public:
    Immediate16(uint16_t value);

    uint16_t read() const override;
    void write(uint16_t value) override;

private:
    const uint16_t value_;
};

class Memory8 : public Operand8 {
public:
    Memory8(memory::Memory *mem, memory::MemoryAddr addr);

    uint8_t read() const override;
    void write(uint8_t value) override;

protected:
    memory::Memory *mem_;
    memory::MemoryAddr addr_;
};

class Memory16 : public Operand16 {
public:
    Memory16(memory::Memory *mem, memory::MemoryAddr addr);

    uint16_t read() const override;
    void write(uint16_t value) override;

protected:
    memory::Memory *mem_;
    memory::MemoryAddr addr_;
};

}  // namespace cpu
