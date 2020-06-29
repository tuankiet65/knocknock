#include "knocknock/interrupt.h"

#include <glog/logging.h>

namespace interrupt {

namespace {

constexpr memory::MemoryAddr IF = 0xff0f;
constexpr memory::MemoryAddr IE = 0xffff;

const std::map<InterruptType, bool> DEFAULT_MAP = {
    {InterruptType::VBLANK, false},
    {InterruptType::SERIAL, false},
    {InterruptType::JOYPAD, false},
    {InterruptType::TIMER, false},
    {InterruptType::LCD_STATUS, false}};

constexpr memory::MemoryValue VBLANK_MASK = 1 << 0;
constexpr memory::MemoryValue LCD_STATUS_MASK = 1 << 1;
constexpr memory::MemoryValue TIMER_MASK = 1 << 2;
constexpr memory::MemoryValue SERIAL_MASK = 1 << 3;
constexpr memory::MemoryValue JOYPAD_MASK = 1 << 4;

memory::MemoryValue to_value(const std::map<InterruptType, bool> &map) {
    // Unused bits are always high.
    memory::MemoryValue result = 0b1110'0000;

    if (map.at(InterruptType::VBLANK))
        result |= VBLANK_MASK;
    if (map.at(InterruptType::LCD_STATUS))
        result |= LCD_STATUS_MASK;
    if (map.at(InterruptType::TIMER))
        result |= TIMER_MASK;
    if (map.at(InterruptType::SERIAL))
        result |= SERIAL_MASK;
    if (map.at(InterruptType::JOYPAD))
        result |= JOYPAD_MASK;

    return result;
}

void from_value(std::map<InterruptType, bool> *map, memory::MemoryValue value) {
    map->at(InterruptType::VBLANK) = value & VBLANK_MASK;
    map->at(InterruptType::LCD_STATUS) = value & LCD_STATUS_MASK;
    map->at(InterruptType::TIMER) = value & TIMER_MASK;
    map->at(InterruptType::SERIAL) = value & SERIAL_MASK;
    map->at(InterruptType::JOYPAD) = value & JOYPAD_MASK;
}

}  // namespace

InterruptController::InterruptController(Interruptible *sink)
    : sink_(sink),
      interrupt_enabled_(DEFAULT_MAP),
      interrupt_requested_(DEFAULT_MAP) {}

bool InterruptController::interrupt(InterruptType reason) {
    interrupt_requested_[reason] = true;

    return true;
}

void InterruptController::tick() {
    for (auto &[reason, requested] : interrupt_requested_) {
        if (requested && interrupt_enabled_[reason]) {
            // Attempt to interrupt the sink
            if (sink_->interrupt(reason)) {
                // Attempt successful, mark that the IRQ is not requested
                // anymore.
                requested = false;
            }
        }
    }
}

memory::MemoryValue InterruptController::read(memory::MemoryAddr addr) const {
    switch (addr) {
        case IF: return to_value(interrupt_requested_);
        case IE: return to_value(interrupt_enabled_);
        default:
            DCHECK(false) << "Invalid read to InterruptController";
            return 0xff;
    }
}

void InterruptController::write(memory::MemoryAddr addr,
                                memory::MemoryValue value) {
    switch (addr) {
        case IF: from_value(&interrupt_requested_, value); break;
        case IE: from_value(&interrupt_enabled_, value); break;
        default: DCHECK(false) << "Invalid write to InterruptController"; break;
    }
}

}  // namespace interrupt
