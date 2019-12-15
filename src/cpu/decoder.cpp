#include "cpu/decoder.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include "memory/memory.h"

namespace cpu {

namespace {

// Bits 7-6 of n
#define X(n) ((n) >> 6)

// Bits 5-3 of n
#define Y(n) (((n)&0b00111000) >> 3)

// Bits 2-0 of n
#define Z(n) ((n)&0b00000111)

#define GET_IMPL(_1, _2, _3, IMPL, ...) IMPL

#define INSTRUCTION(...) GET_IMPL(__VA_ARGS__, INST3, INST2, INST1)(__VA_ARGS__)

#define INST1(__opcode__)                      \
    opcode_ = Instruction::Opcode::__opcode__; \
    lhs_ = Instruction::Operand::None;         \
    rhs_ = Instruction::Operand::None;

#define INST2(__opcode__, __lhs__)             \
    opcode_ = Instruction::Opcode::__opcode__; \
    lhs_ = Instruction::Operand::__lhs__;      \
    rhs_ = Instruction::Operand::None;

#define INST3(__opcode__, __lhs__, __rhs__)    \
    opcode_ = Instruction::Opcode::__opcode__; \
    lhs_ = Instruction::Operand::__lhs__;      \
    rhs_ = Instruction::Operand::__rhs__;

static const Instruction::Operand r[] = {
    Instruction::Operand::B,      // 0
    Instruction::Operand::C,      // 1
    Instruction::Operand::D,      // 2
    Instruction::Operand::E,      // 3
    Instruction::Operand::H,      // 4
    Instruction::Operand::L,      // 5
    Instruction::Operand::PtrHL,  // 6
    Instruction::Operand::A       // 7
};

static const Instruction::Opcode rot[] = {
    Instruction::Opcode::RLC,  // 0
    Instruction::Opcode::RRC,  // 1
    Instruction::Opcode::RL,   // 2
    Instruction::Opcode::RR,   // 3
    Instruction::Opcode::SLA,  // 4
    Instruction::Opcode::SRA,  // 5
    Instruction::Opcode::SLL,  // 6
    Instruction::Opcode::SRL   // 7
};

}  // namespace

// Decode a CB-prefix instruction. |opcode| is the second byte following the
// CB prefix.
void Decoder::decode_cb(uint8_t opcode) {
    lhs_ = r[Z(opcode)];

    switch (X(opcode)) {
        case 0: opcode_ = rot[Y(opcode)]; break;
        case 1: opcode_ = Instruction::Opcode::BIT; break;
        case 2: opcode_ = Instruction::Opcode::RES; break;
        case 3: opcode_ = Instruction::Opcode::SET; break;
        default:
            LOG(ERROR) << fmt::format("Unknown x: {}", X(opcode));
            INSTRUCTION(NOP);
            break;
    }
}

// Decode 8-bit load instructions. LD (HL), (HL) is repurposed as the HALT
// instruction
void Decoder::decode_ld_8bit(uint8_t opcode) {
    DCHECK(X(opcode) == 1) << fmt::format(
        "This opcode is not an 8-bit load instruction: {:#02x}", opcode);

    if (opcode == 0x76) {
        // Special case when both lhs and rhs are (HL)
        INSTRUCTION(HALT);
        return;
    }

    opcode_ = Instruction::Opcode::LD;
    lhs_ = r[Y(opcode)];
    rhs_ = r[Z(opcode)];
}

// Decode ALU instructions.
void Decoder::decode_alu(uint8_t opcode) {
    DCHECK(X(opcode) == 2) << fmt::format(
        "This opcode is not a ALU instruction, [reg8]: {:#02x}", opcode);

    Instruction::Operand reg = r[Z(opcode)];

    switch (Y(opcode)) {
        case 0:  // ADD A, r
            opcode_ = Instruction::Opcode::ADD;
            lhs_ = Instruction::Operand::A;
            rhs_ = reg;
            break;
        case 1:  // ADC A, r
            opcode_ = Instruction::Opcode::ADC;
            lhs_ = Instruction::Operand::A;
            rhs_ = reg;
            break;
        case 2:  // SUB r;
            opcode_ = Instruction::Opcode::SUB;
            lhs_ = reg;
            break;
        case 3:  // SBC a, r
            opcode_ = Instruction::Opcode::SBC;
            lhs_ = Instruction::Operand::A;
            rhs_ = reg;
            break;
        case 4:  // AND r
            opcode_ = Instruction::Opcode::AND;
            lhs_ = reg;
            break;
        case 5:  // XOR r
            opcode_ = Instruction::Opcode::XOR;
            lhs_ = reg;
            break;
        case 6:  // OR r
            opcode_ = Instruction::Opcode::OR;
            lhs_ = reg;
            break;
        case 7:  // CP r
            opcode_ = Instruction::Opcode::CP;
            lhs_ = reg;
            break;
        default:
            LOG(ERROR) << fmt::format("Unknown y: {}", Y(opcode));
            INSTRUCTION(NOP);
            break;
    }
}

void Decoder::decode_assorted(uint8_t opcode) {
    switch (opcode) {
        case 0x00: INSTRUCTION(NOP); break;
        case 0x01: INSTRUCTION(LD, BC, Imm16); break;
        case 0x02: INSTRUCTION(LD, PtrBC, A); break;
        case 0x03: INSTRUCTION(INC, BC); break;
        case 0x04: INSTRUCTION(INC, B); break;
        case 0x05: INSTRUCTION(DEC, B); break;
        case 0x06: INSTRUCTION(LD, B, Imm8); break;
        case 0x07: INSTRUCTION(RLCA); break;
        case 0x08: INSTRUCTION(LD, PtrImm16, SP); break;
        case 0x09: INSTRUCTION(ADD, HL, BC); break;
        case 0x0a: INSTRUCTION(LD, A, PtrBC); break;
        case 0x0b: INSTRUCTION(DEC, BC); break;
        case 0x0c: INSTRUCTION(INC, C); break;
        case 0x0d: INSTRUCTION(DEC, C); break;
        case 0x0e: INSTRUCTION(LD, C, Imm8); break;
        case 0x0f: INSTRUCTION(RRCA); break;
        case 0x10: INSTRUCTION(STOP); break;
        case 0x11: INSTRUCTION(LD, DE, Imm16); break;
        case 0x12: INSTRUCTION(LD, PtrDE, A); break;
        case 0x13: INSTRUCTION(INC, DE); break;
        case 0x14: INSTRUCTION(INC, D); break;
        case 0x15: INSTRUCTION(DEC, D); break;
        case 0x16: INSTRUCTION(LD, D, Imm8); break;
        case 0x17: INSTRUCTION(RLA); break;
        case 0x18: INSTRUCTION(JR, Imm8Sign); break;
        case 0x19: INSTRUCTION(ADD, HL, DE); break;
        case 0x1a: INSTRUCTION(LD, A, PtrDE); break;
        case 0x1b: INSTRUCTION(DEC, DE); break;
        case 0x1c: INSTRUCTION(INC, E); break;
        case 0x1d: INSTRUCTION(DEC, E); break;
        case 0x1e: INSTRUCTION(LD, E, Imm8); break;
        case 0x1f: INSTRUCTION(RRA); break;
        case 0x20: INSTRUCTION(JR, NZ, Imm8Sign); break;
        case 0x21: INSTRUCTION(LD, HL, Imm16); break;
        case 0x22: INSTRUCTION(LD, PtrHLPlus, A); break;
        case 0x23: INSTRUCTION(INC, HL); break;
        case 0x24: INSTRUCTION(INC, H); break;
        case 0x25: INSTRUCTION(DEC, H); break;
        case 0x26: INSTRUCTION(LD, H, Imm8); break;
        case 0x27: INSTRUCTION(DAA); break;
        case 0x28: INSTRUCTION(JR, Z, Imm8Sign); break;
        case 0x29: INSTRUCTION(ADD, HL, HL); break;
        case 0x2a: INSTRUCTION(LD, A, PtrHLPlus); break;
        case 0x2b: INSTRUCTION(DEC, HL); break;
        case 0x2c: INSTRUCTION(INC, L); break;
        case 0x2d: INSTRUCTION(DEC, L); break;
        case 0x2e: INSTRUCTION(LD, L, Imm8); break;
        case 0x2f: INSTRUCTION(CPL); break;
        case 0x30: INSTRUCTION(JR, NC, Imm8Sign); break;
        case 0x31: INSTRUCTION(LD, SP, Imm16); break;
        case 0x32: INSTRUCTION(LD, PtrHLMinus, A); break;
        case 0x33: INSTRUCTION(INC, SP); break;
        case 0x34: INSTRUCTION(INC, PtrHL); break;
        case 0x35: INSTRUCTION(DEC, PtrHL); break;
        case 0x36: INSTRUCTION(LD, PtrHL, Imm8); break;
        case 0x37: INSTRUCTION(SCF); break;
        case 0x38: INSTRUCTION(JR, C, Imm8Sign); break;
        case 0x39: INSTRUCTION(ADD, HL, SP); break;
        case 0x3a: INSTRUCTION(LD, A, PtrHLMinus); break;
        case 0x3b: INSTRUCTION(DEC, SP); break;
        case 0x3c: INSTRUCTION(INC, A); break;
        case 0x3d: INSTRUCTION(DEC, A); break;
        case 0x3e: INSTRUCTION(LD, A, Imm8); break;
        case 0x3f: INSTRUCTION(CCF); break;
        case 0xc0: INSTRUCTION(RET, NZ); break;
        case 0xc1: INSTRUCTION(POP, BC); break;
        case 0xc2: INSTRUCTION(JP, NZ, Imm16); break;
        case 0xc3: INSTRUCTION(JP, Imm16); break;
        case 0xc4: INSTRUCTION(CALL, NZ, Imm16); break;
        case 0xc5: INSTRUCTION(PUSH, BC); break;
        case 0xc6: INSTRUCTION(ADD, A, Imm8); break;
        case 0xc7: INSTRUCTION(RST_00H); break;
        case 0xc8: INSTRUCTION(RET, Z); break;
        case 0xc9: INSTRUCTION(RET); break;
        case 0xca: INSTRUCTION(JP, Z, Imm16); break;
        case 0xcc: INSTRUCTION(CALL, Z, Imm16); break;
        case 0xcd: INSTRUCTION(CALL, Imm16); break;
        case 0xce: INSTRUCTION(ADC, A, Imm8); break;
        case 0xcf: INSTRUCTION(RST_08H); break;
        case 0xd0: INSTRUCTION(RET, NC); break;
        case 0xd1: INSTRUCTION(POP, DE); break;
        case 0xd2: INSTRUCTION(JP, NC, Imm16); break;
        case 0xd4: INSTRUCTION(CALL, NC, Imm16); break;
        case 0xd5: INSTRUCTION(PUSH, DE); break;
        case 0xd6: INSTRUCTION(SUB, Imm8); break;
        case 0xd7: INSTRUCTION(RST_10H); break;
        case 0xd8: INSTRUCTION(RET, C); break;
        case 0xd9: INSTRUCTION(RETI); break;
        case 0xda: INSTRUCTION(JP, C, Imm16); break;
        case 0xdc: INSTRUCTION(CALL, C, Imm16); break;
        case 0xde: INSTRUCTION(SBC, A, Imm8); break;
        case 0xdf: INSTRUCTION(RST_18H); break;
        case 0xe0: INSTRUCTION(LDH, PtrImm8, A); break;
        case 0xe1: INSTRUCTION(POP, HL); break;
        case 0xe2: INSTRUCTION(LD, PtrC, A); break;
        case 0xe5: INSTRUCTION(PUSH, HL); break;
        case 0xe6: INSTRUCTION(AND, Imm8); break;
        case 0xe7: INSTRUCTION(RST_20H); break;
        case 0xe8: INSTRUCTION(ADD, SP, Imm8Sign); break;
        case 0xe9: INSTRUCTION(JP, PtrHL); break;
        case 0xea: INSTRUCTION(LD, PtrImm16, A); break;
        case 0xee: INSTRUCTION(XOR, Imm8); break;
        case 0xef: INSTRUCTION(RST_28H); break;
        case 0xf0: INSTRUCTION(LDH, A, PtrImm8); break;
        case 0xf1: INSTRUCTION(POP, AF); break;
        case 0xf2: INSTRUCTION(LD, A, PtrC); break;
        case 0xf3: INSTRUCTION(DI); break;
        case 0xf5: INSTRUCTION(PUSH, AF); break;
        case 0xf6: INSTRUCTION(OR, Imm8); break;
        case 0xf7: INSTRUCTION(RST_30H); break;
        case 0xf8: INSTRUCTION(LDHL, SP, Imm8Sign); break;
        case 0xf9: INSTRUCTION(LD, SP, HL); break;
        case 0xfa: INSTRUCTION(LD, A, PtrImm16); break;
        case 0xfb: INSTRUCTION(EI); break;
        case 0xfe: INSTRUCTION(CP, Imm8); break;
        case 0xff: INSTRUCTION(RST_38H); break;

        default:
            LOG(ERROR) << fmt::format("Unknown instruction: {:#02x}", opcode);
            INSTRUCTION(NOP);
            break;
    }
}

Decoder::Decoder(const memory::Memory *memory, memory::MemoryAddr *pc)
    : memory_(memory), pc_(pc), state_(State::INITIAL) {}

void Decoder::reset() {
    opcode_ = Instruction::Opcode::NOP;
    lhs_ = Instruction::Operand::None;
    rhs_ = Instruction::Operand::None;
    imm16_ = 0;
    decoded_instruction_.reset();
}

bool Decoder::needs_imm8(Instruction::Operand operand) {
    return (operand == Instruction::Operand::Imm8) ||
           (operand == Instruction::Operand::PtrImm8);
}

bool Decoder::needs_imm8sign(Instruction::Operand operand) {
    return (operand == Instruction::Operand::Imm8Sign);
}

bool Decoder::needs_imm16(Instruction::Operand operand) {
    return (operand == Instruction::Operand::Imm16) ||
           (operand == Instruction::Operand::PtrImm16);
}

void Decoder::step() {
    uint8_t value = memory_->read(*pc_);
    (*pc_)++;

    if (state_ == State::INITIAL) {
        state_ = State::OPCODE;
    }

    if (state_ == State::OPCODE) {
        // Reset the state machine first
        reset();

        uint8_t opcode = value;

        if (opcode == 0xcb) {
            state_ = State::CB_PREFIX;
            return;
        }

        // shortcut for easy to decode opcodes
        switch (X(opcode)) {
            case 1: decode_ld_8bit(opcode); break;
            case 2: decode_alu(opcode); break;
            default: decode_assorted(opcode); break;
        }

        if (needs_imm8sign(lhs_) || needs_imm8sign(rhs_)) {
            state_ = State::IMMEDIATE_8_SIGN;
        } else if (needs_imm8(lhs_) || needs_imm8(rhs_)) {
            state_ = State::IMMEDIATE_8;
        } else if (needs_imm16(lhs_) || needs_imm16(rhs_)) {
            state_ = State::IMMEDIATE_16_LOW;
        } else {
            decoded_instruction_.emplace(opcode_, lhs_, rhs_);
            state_ = State::OPCODE;
        }

        return;
    }

    if (state_ == State::CB_PREFIX) {
        decode_cb(value);
        decoded_instruction_.emplace(opcode_, lhs_, rhs_);
        state_ = State::OPCODE;
        return;
    }

    if (state_ == State::IMMEDIATE_8) {
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, value);
        state_ = State::OPCODE;
        return;
    }

    if (state_ == State::IMMEDIATE_8_SIGN) {
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, (int8_t)(value));
        state_ = State::OPCODE;
        return;
    }

    if (state_ == State::IMMEDIATE_16_LOW) {
        imm16_ = value;
        state_ = State::IMMEDIATE_16_HIGH;
        return;
    }

    if (state_ == State::IMMEDIATE_16_HIGH) {
        imm16_ = ((uint16_t)(value) << 8) | imm16_;
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, imm16_);
        state_ = State::OPCODE;
        return;
    }
}

}  // namespace cpu
