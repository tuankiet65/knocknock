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

#define INST1(__opcode__)                          \
    do {                                           \
        opcode_ = Instruction::Opcode::__opcode__; \
        lhs_ = Instruction::Operand::None;         \
        rhs_ = Instruction::Operand::None;         \
    } while (false)

#define INST2(__opcode__, __lhs__)                 \
    do {                                           \
        opcode_ = Instruction::Opcode::__opcode__; \
        lhs_ = Instruction::Operand::__lhs__;      \
        rhs_ = Instruction::Operand::None;         \
    } while (false)

#define INST3(__opcode__, __lhs__, __rhs__)        \
    do {                                           \
        opcode_ = Instruction::Opcode::__opcode__; \
        lhs_ = Instruction::Operand::__lhs__;      \
        rhs_ = Instruction::Operand::__rhs__;      \
    } while (false)

constexpr Instruction::Operand r[] = {
    Instruction::Operand::B,      // 0
    Instruction::Operand::C,      // 1
    Instruction::Operand::D,      // 2
    Instruction::Operand::E,      // 3
    Instruction::Operand::H,      // 4
    Instruction::Operand::L,      // 5
    Instruction::Operand::PtrHL,  // 6
    Instruction::Operand::A       // 7
};

constexpr Instruction::Opcode rot[] = {
    Instruction::Opcode::RLC,   // 0
    Instruction::Opcode::RRC,   // 1
    Instruction::Opcode::RL,    // 2
    Instruction::Opcode::RR,    // 3
    Instruction::Opcode::SLA,   // 4
    Instruction::Opcode::SRA,   // 5
    Instruction::Opcode::SWAP,  // 6
    Instruction::Opcode::SRL    // 7
};

}  // namespace

// Decode a CB-prefix instruction. |opcode| is the second byte following the
// CB prefix.
bool Decoder::decode_cb(uint8_t opcode) {
    auto rz = r[Z(opcode)];
    uint8_t y = Y(opcode);

    switch (X(opcode)) {
        case 0:
            opcode_ = rot[Y(opcode)];
            lhs_ = rz;
            break;
        case 1:
            opcode_ = Instruction::Opcode::BIT;
            lhs_ = Instruction::Operand::Imm8;
            imm8_ = y;
            rhs_ = rz;
            break;
        case 2:
            opcode_ = Instruction::Opcode::RES;
            lhs_ = Instruction::Operand::Imm8;
            imm8_ = y;
            rhs_ = rz;
            break;
        case 3:
            opcode_ = Instruction::Opcode::SET;
            lhs_ = Instruction::Operand::Imm8;
            imm8_ = y;
            rhs_ = rz;
            break;
        default: return false;
    }

    // Reachable only if the default case is not reached.
    return true;
}

// Decode 8-bit load instructions. LD (HL), (HL) is repurposed as the HALT
// instruction
bool Decoder::decode_ld_8bit(uint8_t opcode) {
    if (X(opcode) != 1) {
        return false;
    }

    // Special case when both lhs and rhs are (HL)
    if (opcode == 0x76) {
        INSTRUCTION(HALT);
    } else {
        opcode_ = Instruction::Opcode::LD;
        lhs_ = r[Y(opcode)];
        rhs_ = r[Z(opcode)];
    }

    return true;
}

// Decode ALU instructions.
bool Decoder::decode_alu(uint8_t opcode) {
    if (X(opcode) != 2) {
        return false;
    }

    auto reg = r[Z(opcode)];

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
        default: return false;
    }

    // Reachable only if the default case is not reached.
    return true;
}

bool Decoder::decode_assorted(uint8_t opcode) {
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
        case 0x20: INSTRUCTION(JR, FlagNZ, Imm8Sign); break;
        case 0x21: INSTRUCTION(LD, HL, Imm16); break;
        case 0x22: INSTRUCTION(LDI, PtrHL, A); break;
        case 0x23: INSTRUCTION(INC, HL); break;
        case 0x24: INSTRUCTION(INC, H); break;
        case 0x25: INSTRUCTION(DEC, H); break;
        case 0x26: INSTRUCTION(LD, H, Imm8); break;
        case 0x27: INSTRUCTION(DAA); break;
        case 0x28: INSTRUCTION(JR, FlagZ, Imm8Sign); break;
        case 0x29: INSTRUCTION(ADD, HL, HL); break;
        case 0x2a: INSTRUCTION(LDI, A, PtrHL); break;
        case 0x2b: INSTRUCTION(DEC, HL); break;
        case 0x2c: INSTRUCTION(INC, L); break;
        case 0x2d: INSTRUCTION(DEC, L); break;
        case 0x2e: INSTRUCTION(LD, L, Imm8); break;
        case 0x2f: INSTRUCTION(CPL); break;
        case 0x30: INSTRUCTION(JR, FlagNC, Imm8Sign); break;
        case 0x31: INSTRUCTION(LD, SP, Imm16); break;
        case 0x32: INSTRUCTION(LDD, PtrHL, A); break;
        case 0x33: INSTRUCTION(INC, SP); break;
        case 0x34: INSTRUCTION(INC, PtrHL); break;
        case 0x35: INSTRUCTION(DEC, PtrHL); break;
        case 0x36: INSTRUCTION(LD, PtrHL, Imm8); break;
        case 0x37: INSTRUCTION(SCF); break;
        case 0x38: INSTRUCTION(JR, FlagC, Imm8Sign); break;
        case 0x39: INSTRUCTION(ADD, HL, SP); break;
        case 0x3a: INSTRUCTION(LDD, A, PtrHL); break;
        case 0x3b: INSTRUCTION(DEC, SP); break;
        case 0x3c: INSTRUCTION(INC, A); break;
        case 0x3d: INSTRUCTION(DEC, A); break;
        case 0x3e: INSTRUCTION(LD, A, Imm8); break;
        case 0x3f: INSTRUCTION(CCF); break;
        case 0xc0: INSTRUCTION(RET, FlagNZ); break;
        case 0xc1: INSTRUCTION(POP, BC); break;
        case 0xc2: INSTRUCTION(JP, FlagNZ, Imm16); break;
        case 0xc3: INSTRUCTION(JP, Imm16); break;
        case 0xc4: INSTRUCTION(CALL, FlagNZ, Imm16); break;
        case 0xc5: INSTRUCTION(PUSH, BC); break;
        case 0xc6: INSTRUCTION(ADD, A, Imm8); break;
        case 0xc8: INSTRUCTION(RET, FlagZ); break;
        case 0xc9: INSTRUCTION(RET); break;
        case 0xca: INSTRUCTION(JP, FlagZ, Imm16); break;
        case 0xcc: INSTRUCTION(CALL, FlagZ, Imm16); break;
        case 0xcd: INSTRUCTION(CALL, Imm16); break;
        case 0xce: INSTRUCTION(ADC, A, Imm8); break;
        case 0xd0: INSTRUCTION(RET, FlagNC); break;
        case 0xd1: INSTRUCTION(POP, DE); break;
        case 0xd2: INSTRUCTION(JP, FlagNC, Imm16); break;
        case 0xd4: INSTRUCTION(CALL, FlagNC, Imm16); break;
        case 0xd5: INSTRUCTION(PUSH, DE); break;
        case 0xd6: INSTRUCTION(SUB, Imm8); break;
        case 0xd8: INSTRUCTION(RET, FlagC); break;
        case 0xd9: INSTRUCTION(RETI); break;
        case 0xda: INSTRUCTION(JP, FlagC, Imm16); break;
        case 0xdc: INSTRUCTION(CALL, FlagC, Imm16); break;
        case 0xde: INSTRUCTION(SBC, A, Imm8); break;
        case 0xe0: INSTRUCTION(LD, PtrImm8, A); break;
        case 0xe1: INSTRUCTION(POP, HL); break;
        case 0xe2: INSTRUCTION(LD, PtrC, A); break;
        case 0xe5: INSTRUCTION(PUSH, HL); break;
        case 0xe6: INSTRUCTION(AND, Imm8); break;
        case 0xe8: INSTRUCTION(ADD, SP, Imm8Sign); break;
        case 0xe9: INSTRUCTION(JP, HL); break;
        case 0xea: INSTRUCTION(LD, PtrImm16, A); break;
        case 0xee: INSTRUCTION(XOR, Imm8); break;
        case 0xf0: INSTRUCTION(LD, A, PtrImm8); break;
        case 0xf1: INSTRUCTION(POP, AF); break;
        case 0xf2: INSTRUCTION(LD, A, PtrC); break;
        case 0xf3: INSTRUCTION(DI); break;
        case 0xf5: INSTRUCTION(PUSH, AF); break;
        case 0xf6: INSTRUCTION(OR, Imm8); break;
        case 0xf8: INSTRUCTION(LDHL, SP, Imm8Sign); break;
        case 0xf9: INSTRUCTION(LD, SP, HL); break;
        case 0xfa: INSTRUCTION(LD, A, PtrImm16); break;
        case 0xfb: INSTRUCTION(EI); break;
        case 0xfe: INSTRUCTION(CP, Imm8); break;

        default: return false;
    }

    // Reachable only if the default case is not reached.
    return true;
}

bool Decoder::decode_rst(uint8_t opcode) {
    switch (opcode) {
        case 0xc7:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x00;
            break;
        case 0xcf:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x08;
            break;
        case 0xd7:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x10;
            break;
        case 0xdf:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x18;
            break;
        case 0xe7:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x20;
            break;
        case 0xef:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x28;
            break;
        case 0xf7:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x30;
            break;
        case 0xff:
            INSTRUCTION(RST, Imm8);
            imm8_ = 0x38;
            break;
        default: return false;
    }
    // Reachable only if the default case is not reached.
    return true;
}

Decoder::Decoder(const memory::Memory *memory, memory::MemoryAddr *pc)
    : state_(State::OPCODE), memory_(memory), pc_(pc) {}

void Decoder::reset() {
    opcode_ = Instruction::Opcode::NOP;
    lhs_ = Instruction::Operand::None;
    rhs_ = Instruction::Operand::None;
    imm8_.reset();
    imm8sign_.reset();
    imm16_.reset();
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

    if (state_ == State::OPCODE) {
        // Reset the state machine first
        reset();

        uint8_t opcode = value;

        if (opcode == 0xcb) {
            state_ = State::CB_PREFIX;
            return;
        }

        // No immediates follow these instructions.
        // This expression will be short-circuited: if any of the decode
        // succeeds, then others down the line will not be run.
        if (decode_rst(opcode) || decode_ld_8bit(opcode) ||
            decode_alu(opcode)) {
            assemble();
            return;
        }

        if (decode_assorted(opcode)) {
            // Determine whether to read the next immediate or not.
            if (needs_imm8sign(lhs_) || needs_imm8sign(rhs_)) {
                state_ = State::IMMEDIATE_8_SIGN;
            } else if (needs_imm8(lhs_) || needs_imm8(rhs_)) {
                state_ = State::IMMEDIATE_8;
            } else if (needs_imm16(lhs_) || needs_imm16(rhs_)) {
                state_ = State::IMMEDIATE_16_LOW;
            } else {
                assemble();
            }
            return;
        }

        // If decoded is false, then the opcode is invalid.
        LOG(ERROR) << fmt::format("Unknown opcode: {:#02x}, assuming NOP",
                                  opcode);
        INSTRUCTION(NOP);
        return;
    }

    if (state_ == State::CB_PREFIX) {
        if (!decode_cb(value)) {
            DCHECK(false) << "Error decoding CB prefix opcode";
            INSTRUCTION(NOP);
        }

        assemble();
        return;
    }

    if (state_ == State::IMMEDIATE_8) {
        imm8_ = value;
        assemble();
        return;
    }

    if (state_ == State::IMMEDIATE_8_SIGN) {
        imm8sign_ = (int8_t)(value);
        assemble();
        return;
    }

    if (state_ == State::IMMEDIATE_16_LOW) {
        imm16_ = value;
        state_ = State::IMMEDIATE_16_HIGH;
        return;
    }

    if (state_ == State::IMMEDIATE_16_HIGH) {
        DCHECK(imm16_.has_value()) << "imm16_ does not have any value";
        imm16_ = ((uint16_t)(value) << 8) | (*imm16_);
        assemble();
        return;
    }
}

void Decoder::assemble() {
    if (imm8_.has_value()) {
        DCHECK((!imm8sign_.has_value()) && (!imm16_.has_value()))
            << "imm8_ has a value, and imm8sign_ or imm16_ also have "
               "values";
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, *imm8_);
    } else if (imm8sign_.has_value()) {
        DCHECK((!imm8_.has_value()) && (!imm16_.has_value()))
            << "imm8sign_ has a value, and imm8_ or imm16_ also have "
               "values";
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, *imm8sign_);
    } else if (imm16_.has_value()) {
        DCHECK((!imm8_.has_value()) && (!imm8sign_.has_value()))
            << "imm16_ has a value, and imm8_ or imm8sign_ also have "
               "values";
        decoded_instruction_.emplace(opcode_, lhs_, rhs_, *imm16_);
    } else {
        decoded_instruction_.emplace(opcode_, lhs_, rhs_);
    }

    state_ = State::OPCODE;
}

}  // namespace cpu
