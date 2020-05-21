#pragma once

#include "knocknock/memory/memory.h"

namespace cpu {

template <class T>
class OperandT {
public:
    OperandT() {}

    virtual T read() const = 0;
    virtual void write(T value) = 0;

    virtual ~OperandT() {}

    // Disable copy and move constructor
    OperandT(const OperandT<T> &) = delete;
    OperandT(OperandT<T> &&) = delete;
};

using Operand8 = OperandT<uint8_t>;
using Operand8Sign = OperandT<int8_t>;
using Operand16 = OperandT<uint16_t>;

class Register8 : public Operand8 {
public:
    Register8();

    uint8_t read() const override;
    void write(uint8_t value) override;

private:
    uint8_t value_;
};

class FlagRegister : public Operand8 {
public:
    FlagRegister();

    uint8_t read() const override;
    void write(uint8_t value) override;

    bool zero;
    bool subtract;
    bool half_carry;
    bool carry;
};

class Register8Sign : public Operand8Sign {
public:
    Register8Sign();

    int8_t read() const override;
    void write(int8_t value) override;

private:
    int8_t value_;
};

class Register16 : public Operand16 {
public:
    Register16();

    uint16_t read() const override;
    void write(uint16_t value) override;

private:
    uint16_t value_;
};

class Register16Mirror : public Operand16 {
public:
    Register16Mirror(Operand8 *msb, Operand8 *lsb);

    uint16_t read() const override;
    void write(uint16_t value) override;

private:
    Operand8 *msb_, *lsb_;
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
    Memory8(memory::Memory *mem, const Operand16 &addr);

    uint8_t read() const override;
    void write(uint8_t value) override;

protected:
    memory::Memory *mem_;
    const Operand16 &addr_;
};

class Memory8HiMem : public Operand8 {
public:
    Memory8HiMem(memory::Memory *mem, const Operand8 &addr);

    uint8_t read() const override;
    void write(uint8_t value) override;

protected:
    memory::Memory *mem_;
    const Operand8 &addr_;
};

class Memory16 : public Operand16 {
public:
    Memory16(memory::Memory *mem, const Operand16 &addr);

    uint16_t read() const override;
    void write(uint16_t value) override;

protected:
    memory::Memory *mem_;
    const Operand16 &addr_;
};

}  // namespace cpu
