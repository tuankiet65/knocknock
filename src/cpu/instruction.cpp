#include "cpu/instruction.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <optional>

#include "memory/memory.h"

namespace cpu {

namespace {

Instruction decode_cb(const memory::Memory *mem, memory::MemoryAddr *addr) {
    uint8_t _opcode = mem->read(*addr);
    (*addr)++;

    uint8_t x = _opcode >> 6,             // first two bits
        y = (_opcode & 0b00111000) >> 3,  // middle three bits
        z = _opcode & 0b00000111;         // last three bits

    Instruction::Operand lhs;
    switch (z) {
        case 0: lhs = Instruction::Operand::RegB; break;
        case 1: lhs = Instruction::Operand::RegC; break;
        case 2: lhs = Instruction::Operand::RegD; break;
        case 3: lhs = Instruction::Operand::RegE; break;
        case 4: lhs = Instruction::Operand::RegH; break;
        case 5: lhs = Instruction::Operand::RegL; break;
        case 6: lhs = Instruction::Operand::MemRegHL; break;
        case 7: lhs = Instruction::Operand::RegA; break;
        default: DCHECK(false) << fmt::format("Unknown z: {}", z); break;
    }

    Instruction::Opcode opcode;
    switch (x) {
        case 0:
            switch (y) {
                case 0: opcode = Instruction::Opcode::RLC; break;
                case 1: opcode = Instruction::Opcode::RRC; break;
                case 2: opcode = Instruction::Opcode::RL; break;
                case 3: opcode = Instruction::Opcode::RR; break;
                case 4: opcode = Instruction::Opcode::SLA; break;
                case 5: opcode = Instruction::Opcode::SRA; break;
                case 6: opcode = Instruction::Opcode::SLL; break;
                case 7: opcode = Instruction::Opcode::SRL; break;
                default:
                    DCHECK(false) << fmt::format("Unknown y: {}", y);
                    break;
            }
            break;
        case 1: opcode = Instruction::Opcode::BIT; break;
        case 2: opcode = Instruction::Opcode::RES; break;
        case 3: opcode = Instruction::Opcode::SET; break;
        default: DCHECK(false) << fmt::format("Unknown x: {}", x); break;
    }

    return Instruction(opcode, lhs, Instruction::Operand::None);
}

std::string disassemble_opcode(Instruction::Opcode opcode) {
    switch (opcode) {
        case Instruction::Opcode::RLC: return "RLC";
        case Instruction::Opcode::RRC: return "RRC";
        case Instruction::Opcode::RL: return "RL";
        case Instruction::Opcode::RR: return "RR";
        case Instruction::Opcode::SLA: return "SLA";
        case Instruction::Opcode::SRA: return "SRA";
        case Instruction::Opcode::SLL: return "SLL";
        case Instruction::Opcode::SRL: return "SRL";
        case Instruction::Opcode::BIT: return "BIT";
        case Instruction::Opcode::RES: return "RES";
        case Instruction::Opcode::SET: return "SET";
    }

    DCHECK(false) << fmt::format("Unknown opcode: {}", opcode);
    return fmt::format("[unknown opcode: {}]", opcode);
}

std::string disassemble_operand(Instruction::Operand operand) {
    switch (operand) {
        case Instruction::Operand::RegA: return "A";
        case Instruction::Operand::RegB: return "B";
        case Instruction::Operand::RegC: return "C";
        case Instruction::Operand::RegD: return "D";
        case Instruction::Operand::RegE: return "E";
        case Instruction::Operand::RegH: return "H";
        case Instruction::Operand::RegL: return "L";
        case Instruction::Operand::MemRegHL: return "(HL)";
    }
}

}  // namespace

Instruction::Instruction(Opcode opcode, Operand lhs, Operand rhs)
    : opcode_(opcode), lhs_(lhs), rhs_(rhs) {}

// static

std::optional<Instruction> Instruction::decode(const memory::Memory *mem,
                                               memory::MemoryAddr *addr) {
    uint8_t opcode = mem->read(*addr);
    (*addr)++;

    if (opcode == 0xcb)
        return decode_cb(mem, addr);
}

std::string Instruction::disassemble() const {
    std::string result = disassemble_opcode(opcode_);

    if (lhs_ != Instruction::Operand::None)
        result += " " + disassemble_operand(lhs_);

    if (rhs_ != Instruction::Operand::None)
        result += ", " + disassemble_operand(rhs_);

    return result;
}

}  // namespace cpu
