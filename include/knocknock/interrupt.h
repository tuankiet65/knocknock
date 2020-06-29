#pragma once

#include <map>

#include "knocknock/memory/memory.h"
#include "knocknock/peripherals/tickable.h"

namespace interrupt {

/**
 * Enum of all possible interrupt types.
 */
enum class InterruptType {
    /**
     * Vertical blank.
     */
    VBLANK,

    /**
     * LCD Status (for example, mode change event or LY == LYC).
     */
    LCD_STATUS,

    /**
     * Timer.
     */
    TIMER,

    /**
     * Serial.
     */
    SERIAL,

    /**
     * Joypad (when any of the P10-P13 pins go low).
     */
    JOYPAD,
};

/**
 * Interface for components that can be interrupted, for example, the CPU.
 */
class Interruptible {
public:
    /**
     * Attempt to interrupt the component.
     * @param reason reason of the interrupt.
     * @return true if the interrupt was handled, false otherwise. For example,
     *         when interrupt is disabled, the CPU can't handle any interrupt
     *         requests.
     */
    virtual bool interrupt(InterruptType reason) = 0;

    virtual ~Interruptible() = default;
};

class InterruptController : public Interruptible,
                            public memory::Memory,
                            public peripherals::Tickable {
public:
    InterruptController(Interruptible *sink);

    // Interruptible::
    bool interrupt(InterruptType reason) override;

    // memory::Memory
    memory::MemoryValue read(memory::MemoryAddr addr) const override;
    void write(memory::MemoryAddr addr, memory::MemoryValue value) override;

    // clock::Tickable::
    void tick() override;

private:
    Interruptible *sink_;

    std::map<InterruptType, bool> interrupt_enabled_;
    std::map<InterruptType, bool> interrupt_requested_;
};

}  // namespace interrupt
