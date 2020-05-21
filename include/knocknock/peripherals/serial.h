#pragma once

#include <optional>
#include <queue>

#include "knocknock/memory/memory.h"
#include "knocknock/memory/mmu.h"
#include "knocknock/peripherals/tickable.h"

namespace peripherals {

/**
 * Handles serial communication to the Gameboy. The class maintains a queue
 * buffer for incoming and outgoing data.
 */
class Serial : public memory::Memory, public peripherals::Tickable {
public:
    /**
     * Initialize a new Serial.
     */
    Serial();

    // Memory::
    memory::MemoryValue read(memory::MemoryAddr addr) const override;
    void write(memory::MemoryAddr addr, memory::MemoryValue value) override;

    /**
     * Queue a byte to be sent to the Game Boy.
     * @param value value to be sent.
     */
    void send(uint8_t value);

    /**
     * Retrieve a byte sent from the Game Boy, if any.
     * @return received byte, or an empty std::optional.
     */
    std::optional<uint8_t> receive();

    // Tickable::
    void tick() override;

    void register_to_mmu(memory::MMU *mmu);

private:
    /**
     * Prepare the Serial for a send/receive session.
     */
    void begin_session();

    void end_session();

    /**
     * Whether the Serial is in a send/receive session.
     */
    bool is_in_session_;

    memory::MemoryValue data_;
    uint8_t control_;

    // From outside to Gameboy
    std::queue<uint8_t> in_buffer_;
    uint8_t current_in_byte_;

    // From Gameboy to outside
    std::queue<uint8_t> out_buffer_;
    uint8_t current_out_byte_;

    uint8_t remaining_bits_;
};

}  // namespace peripherals
