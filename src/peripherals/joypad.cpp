#include "joypad.h"

#include <fmt/format.h>

namespace peripherals {

namespace {

constexpr memory::MemoryAddr P1_ADDR = 0xff00;

// Masks for bits in P1 register.
constexpr uint8_t P10_MASK = 1u << 0u;
constexpr uint8_t P11_MASK = 1u << 1u;
constexpr uint8_t P12_MASK = 1u << 2u;
constexpr uint8_t P13_MASK = 1u << 3u;
constexpr uint8_t P14_MASK = 1u << 4u;
constexpr uint8_t P15_MASK = 1u << 5u;

}  // namespace

Joypad::Joypad() : state_(), p14_pulled_down_(false), p15_pulled_down_(false) {
    // Initialize the state of all buttons in the enum
    state_[Button::A] = false;
    state_[Button::B] = false;
    state_[Button::SELECT] = false;
    state_[Button::START] = false;
    state_[Button::UP] = false;
    state_[Button::DOWN] = false;
    state_[Button::LEFT] = false;
    state_[Button::RIGHT] = false;
}

void Joypad::register_to_mmu(memory::MMU *mmu) {
    mmu->register_region(this, P1_ADDR, P1_ADDR);
}

void Joypad::write(memory::MemoryAddr addr, memory::MemoryValue value) {
    if (addr != P1_ADDR) {
        DCHECK(false) << fmt::format("Invalid write to joypad at addr {:#04x}",
                                     addr);
        return;
    }

    p14_pulled_down_ = !(value & P14_MASK);
    p15_pulled_down_ = !(value & P15_MASK);
}

memory::MemoryValue Joypad::read(memory::MemoryAddr addr) const {
    if (addr != P1_ADDR) {
        DCHECK(false) << fmt::format("Invalid read from joypad at addr {:#04x}",
                                     addr);
        return 0xff;
    }

    return get_p1();
}

uint8_t Joypad::get_p1() const {
    // Assume all inputs are pulled up initially.
    uint8_t p1 =
        P15_MASK | P14_MASK | P13_MASK | P12_MASK | P11_MASK | P10_MASK;

    // If P14 is pulled low
    if (p14_pulled_down_) {
        // Set P14 bit to 0.
        p1 &= ~P14_MASK;

        // If a button is pressed, then their respective bits become 0 because
        // their signal is pulled low.
        if (state_.at(Button::RIGHT))
            p1 &= ~P10_MASK;
        if (state_.at(Button::LEFT))
            p1 &= ~P11_MASK;
        if (state_.at(Button::UP))
            p1 &= ~P12_MASK;
        if (state_.at(Button::DOWN))
            p1 &= ~P13_MASK;
    }

    // If P15 is pulled low
    if (p15_pulled_down_) {
        // Set P15 bit to 0.
        p1 &= ~P15_MASK;

        // If a button is pressed, then their respective bits become 0 because
        // their signal is pulled low.
        if (state_.at(Button::A))
            p1 &= ~P10_MASK;
        if (state_.at(Button::B))
            p1 &= ~P11_MASK;
        if (state_.at(Button::SELECT))
            p1 &= ~P12_MASK;
        if (state_.at(Button::START))
            p1 &= ~P13_MASK;
    }

    return p1;
}

void Joypad::press(Button button) {
    state_[button] = true;
}

void Joypad::release(Button button) {
    state_[button] = false;
}

bool Joypad::is_pressed(Button button) {
    return state_[button];
}

}  // namespace peripherals
