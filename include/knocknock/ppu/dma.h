#pragma once

#include "knocknock/memory/memory.h"
#include "knocknock/memory/mmu.h"
#include "knocknock/peripherals/tickable.h"

namespace ppu {

class DMA : public memory::Memory, public peripherals::Tickable {
public:
    DMA(memory::Memory *memory);

    void register_to_mmu(memory::MMU *mmu);

    // memory::Memory::
    memory::MemoryValue read(memory::MemoryAddr addr) const override;
    void write(memory::MemoryAddr addr, memory::MemoryValue value) override;

    // peripherals::Tickable::
    void tick() override;

private:
    memory::Memory *memory_;

    /**
     * Whether the DMA is transferring data or not.
     */
    bool transferring_;

    /**
     * Source address of the current transfer.
     */
    memory::MemoryAddr source_addr_;

    /**
     * Offset of the byte being transferred by the DMA, relative to the source
     * address.
     */
    uint8_t current_byte_;
};

}  // namespace ppu
