#pragma once

#include <unordered_map>

#include "knocknock/memory/memory.h"
#include "knocknock/memory/mmu.h"

namespace peripherals {

class Joypad : public memory::Memory {
public:
    Joypad();

    enum class Button { A, B, SELECT, START, UP, DOWN, LEFT, RIGHT };

    // Memory::
    memory::MemoryValue read(memory::MemoryAddr addr) const override;
    void write(memory::MemoryAddr addr, memory::MemoryValue value) override;

    void register_to_mmu(memory::MMU *mmu);

    void press(Button button);
    void release(Button button);
    bool is_pressed(Button button);

private:
    uint8_t get_p1() const;

    std::unordered_map<Button, bool> state_;

    bool p14_pulled_down_;
    bool p15_pulled_down_;
};

}  // namespace peripherals
