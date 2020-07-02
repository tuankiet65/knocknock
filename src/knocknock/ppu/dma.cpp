#include "knocknock/ppu/dma.h"

#include <glog/logging.h>

namespace ppu {

namespace {

constexpr memory::MemoryAddr DMA_ADDR = 0xff46;
constexpr memory::MemoryAddr OAM_BEGIN = 0xfe00;
constexpr uint8_t OAM_SIZE = 0xa0;

}  // namespace

DMA::DMA(memory::Memory *memory)
    : memory_(memory), transferring_(false), source_addr_(), current_byte_() {}

void DMA::register_to_mmu(memory::MMU *mmu) {
    mmu->register_region(this, DMA_ADDR, DMA_ADDR);
}

memory::MemoryValue DMA::read(memory::MemoryAddr addr) const {
    if (addr != DMA_ADDR) {
        DCHECK(false) << "Invalid read to DMA, returning dummy";
        return 0xff;
    }

    if (transferring_) {
        LOG(ERROR) << "Read to DMA during transfer, returning dummy";
        return 0xff;
    }

    return source_addr_;
}

void DMA::write(memory::MemoryAddr addr, memory::MemoryValue value) {
    if (addr != DMA_ADDR) {
        DCHECK(false) << "Invalid write to DMA, ignoring";
        return;
    }

    if (transferring_) {
        LOG(ERROR) << "Write to DMA during transfer, ignoring";
        return;
    }

    // Initiate a transfer.
    transferring_ = true;
    // Value written is the upper byte of the source address.
    source_addr_ = static_cast<uint16_t>(value) << 8;
    current_byte_ = 0;
}

void DMA::tick() {
    if (!transferring_) {
        return;
    }

    memory_->write(OAM_BEGIN + current_byte_,
                   memory_->read(source_addr_ + current_byte_));
    current_byte_++;
    if (current_byte_ == OAM_SIZE) {
        transferring_ = false;
    }
}

}  // namespace ppu
